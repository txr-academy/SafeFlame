/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mq2_sensor.h"
#include "mq4_sensor.h"
#include "mq7_sensor.h"
#include "hx711.h"
#include "dht22.h"
#include "actuator.h"
#include"LCD.h"
#include "stm32f4xx_hal.h"
#include<stdio.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    LEAK_NORMAL = 0,   // No leak
    LEAK_SLOW   = 1,   // Slow leak
    LEAK_SUDDEN = 2    // Sudden leak
} LeakType_t;
typedef struct {
	    int32_t hx711_value;//hx711 outputs data in 2's complement format.
	    float weight;
	    uint32_t mq2_value;
	    float mq2_ppm;
	    uint32_t mq4_value;
	    float mq4_ppm;
	    uint32_t mq7_value;
	    float mq7_ppm;
	    float temperature;
	    float humidity;
} SensorData_t;

typedef struct {
	LeakType_t leakType;
    float remainingGas;    // kg
    int anomalyFlag;       // 0/1
    float compensatedGas;  // after environmental compensation
    float humidity,temperature;
    float weight;
    float mq2_ppm,mq4_ppm,mq7_ppm;
} StatusData_t;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask05Handle;
osThreadId myTask06Handle;
osThreadId myTask07Handle;
osMessageQId SensorQueueHandle;
osMessageQId StatusQueueHandle;
osMessageQId ControlQueueHandle;
osMessageQId CommQueueHandle;
osMessageQId CompQueueHandle;
osMessageQId LCDQueueHandle;
/* USER CODE BEGIN PV */

/*----------------- HX711 calibration values (temporary)----------------------*/
volatile long offset = 0;             // tare offset (baseline)
volatile float factor = 0.00005f;     // ready-made calibration factor


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
void StartDefaultTask(void const * argument);
void SensorTask(void const * argument);
void ProcessTask(void const * argument);
void ControlTask(void const * argument);
void CommunicationTask(void const * argument);
void CompensationTask(void const * argument);
void lcd(void const * argument);

/* USER CODE BEGIN PFP */

/*---------------------UART Redirect------------------------------------------------*/
int __io_putchar(int ch)
 {
     HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
     return ch;
 }



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*-------------------I2C scanner code-----------------------------------------*/
/*
void I2C_Scanner(void) {
    printf("Starting I2C scan...\r\n");
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 1, 10) == HAL_OK) {
            printf("Found device at 0x%X\r\n", addr);
        }
    }
    printf("I2C scan complete.\r\n");
}
*/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* --------------DWT counter enabled----------------------------------------*/
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /*---------------------------------------------------------------------------*/
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

 // I2C_Scanner();   // Function call to identify slave address.
  HX711_Init();
  lcd_init();

  /*---------------------- HX711 tare offset (empty load cell)----------------*/
offset = HX711_Tare(20);   // average of 20 samples
factor = 0.00005f;// Use ready-made calibration factor for now
/*----------------------------------------------------------------------------*/
printf("********************************************\r\n");
printf("*          System Started                  * \r\n");
printf("*                                          * \r\n");
printf("********************************************\r\n");


  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of SensorQueue */
  osMessageQDef(SensorQueue, 16, 20);
  SensorQueueHandle = osMessageCreate(osMessageQ(SensorQueue), NULL);

  /* definition and creation of StatusQueue */
  osMessageQDef(StatusQueue, 16, 12);
  StatusQueueHandle = osMessageCreate(osMessageQ(StatusQueue), NULL);

  /* definition and creation of ControlQueue */
  osMessageQDef(ControlQueue, 16, uint32_t);
  ControlQueueHandle = osMessageCreate(osMessageQ(ControlQueue), NULL);

  /* definition and creation of CommQueue */
  osMessageQDef(CommQueue, 8, uint32_t);
  CommQueueHandle = osMessageCreate(osMessageQ(CommQueue), NULL);

  /* definition and creation of CompQueue */
  osMessageQDef(CompQueue, 8, uint32_t);
  CompQueueHandle = osMessageCreate(osMessageQ(CompQueue), NULL);

  /* definition and creation of LCDQueue */
  osMessageQDef(LCDQueue, 8, uint32_t);
  LCDQueueHandle = osMessageCreate(osMessageQ(LCDQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, SensorTask, osPriorityNormal, 0, 256);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, ProcessTask, osPriorityRealtime, 0, 256);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, ControlTask, osPriorityHigh, 0, 256);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadDef(myTask05, CommunicationTask, osPriorityLow, 0, 256);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadDef(myTask06, CompensationTask, osPriorityLow, 0, 128);
  myTask06Handle = osThreadCreate(osThread(myTask06), NULL);

  /* definition and creation of myTask07 */
  osThreadDef(myTask07, lcd, osPriorityNormal, 0, 256);
  myTask07Handle = osThreadCreate(osThread(myTask07), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SensorTask */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SensorTask */
void SensorTask(void const * argument)
{
  /* USER CODE BEGIN SensorTask */
	static SensorData_t data;

  /* Infinite loop */
  for(;;)
  {

	  // HX711 (wait for DOUT low handled inside HX711_Read)
	 	         data.hx711_value = HX711_Read();
	 	         data.weight = HX711_GetWeight(offset, factor);

	 	         //MQ2
	 	          data.mq2_value = MQ2_ReadADC();
	 	          data.mq2_ppm = MQ2_GetPPM(data.mq2_value);

	 	          // MQ4
	 	          data.mq4_value = MQ4_ReadADC();
	 	          data.mq4_ppm = MQ4_GetPPM(data.mq4_value);

	 	          // MQ7
	 	           data.mq7_value = MQ7_ReadADC();
	 	           data.mq7_ppm = MQ7_GetPPM(data.mq7_value);

	 	          // DHT22
	              if (DHT22_Read(GPIOA, GPIO_PIN_0, &data.temperature, &data.humidity) == HAL_OK)

	 	          // Push sensor packet into queue
	 	          osMessagePut(SensorQueueHandle, (uint32_t)&data, 0);
	              osDelay(1000); // print/update every 1 second

  }
  /* USER CODE END SensorTask */
}

/* USER CODE BEGIN Header_ProcessTask */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ProcessTask */
void ProcessTask(void const * argument)
{
  /* USER CODE BEGIN ProcessTask */
	    osEvent evt;
	    SensorData_t sensor;
	    static float prevWeight = 12.0f;
	    static StatusData_t status;

    /* Infinite loop */
    for(;;)
    {
    	            evt = osMessageGet(SensorQueueHandle, osWaitForever);
    	            if (evt.status == osEventMessage) {
    	        	SensorData_t *sensorPtr = (SensorData_t *)evt.value.p;
    	        	sensor = *sensorPtr;
    	            // Leak classification
    	            float deltaW = sensor.weight - prevWeight;
    	            float rate = deltaW / 0.5f; // per 0.5s sample
    	            if (rate < -2.0f) status.leakType = LEAK_SUDDEN;
    	            else if (rate < -0.2f) status.leakType = LEAK_SLOW;
    	            else status.leakType = LEAK_NORMAL;

    	            // Prediction
    	            status.remainingGas = sensor.weight / 0.5f;
    	            status.anomalyFlag = 0;
    	            status.compensatedGas = sensor.weight;
    	            status.humidity = sensor.humidity;
    	            status.temperature=sensor.temperature;
    	            status.weight = sensor.weight;
    	            status.mq2_ppm=sensor.mq2_ppm;
    	            status.mq4_ppm=sensor.mq4_ppm;
    	            status.mq7_ppm=sensor.mq7_ppm;
    	            prevWeight = sensor.weight;
    	            osMessagePut(StatusQueueHandle, (uint32_t)&status, 0);
    	            osMessagePut(ControlQueueHandle, (uint32_t)&status, 0);
    	            osMessagePut(CommQueueHandle, (uint32_t)&status, 0);
    	            osMessagePut(CompQueueHandle, (uint32_t)&status, 0);
    	            osMessagePut(LCDQueueHandle, (uint32_t)&status, 0);
                    }
    }
  /* USER CODE END ProcessTask */
}

/* USER CODE BEGIN Header_ControlTask */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ControlTask */
void ControlTask(void const * argument)
{
  /* USER CODE BEGIN ControlTask */
	osEvent evt;
    StatusData_t status;
  /* Infinite loop */
  for(;;)
  {
	  evt = osMessageGet(ControlQueueHandle,osWaitForever);
	          if (evt.status == osEventMessage) {
	          StatusData_t *statusPtr = (StatusData_t *)evt.value.p;
	       	  status = *statusPtr;   // copy into local struct
	              switch (status.leakType) {
	                  case LEAK_SUDDEN:
	                      // closeValve();
	                      // soundBuzzer();
	                      break;

	                  case LEAK_SLOW:
	                      // soundBuzzer();
	                      break;

	                  case LEAK_NORMAL:
	                      // No action
	                      break;
                  }
	          }

    osDelay(1);
  }

  /* USER CODE END ControlTask */
}

/* USER CODE BEGIN Header_CommunicationTask */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CommunicationTask */
void CommunicationTask(void const * argument)
{
  /* USER CODE BEGIN CommunicationTask */
	osEvent evt;
	    StatusData_t status;
	    SensorData_t sensor;
	    const char* leakTypeStr[] = {"Normal", "Slow", "Sudden"};

	/* Infinite loop */
  for(;;)
  {
	  evt = osMessageGet(CommQueueHandle, osWaitForever);
	  	       if (evt.status == osEventMessage) {
	  	       StatusData_t *statusPtr = (StatusData_t *)evt.value.p;
	  	       status = *statusPtr;

	  	       // Fetch the latest sensor data from bundled status queue
	  	       printf("HX711 Weight=%.2f kg\r\n",status.weight);
	  	       printf("MQ2 sensor PPM =%.2f\r\n",status.mq2_ppm);
	  	       printf("MQ4 sensor PPM =%.2f\r\n",status.mq4_ppm);
	  	       printf("MQ7 sensor PPM =%.2f\r\n",status.mq7_ppm);
	  	       printf("DHT22: Temp=%.1f °C, Hum=%.1f %%\r\n", status.temperature, status.humidity);
	  	       }
	  	       // Print status information like leak status,remaining number of days etc.
	  	       printf("LeakType=%s, Remaining=%.1f kg, Compensated=%.2f kg\r\n",leakTypeStr[status.leakType], status.remainingGas, status.compensatedGas);
	  	       osDelay(10000); // print every 10 seconds
	          }


  /* USER CODE END CommunicationTask */
}

/* USER CODE BEGIN Header_CompensationTask */
/**
* @brief Function implementing the myTask06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CompensationTask */
void CompensationTask(void const * argument)
{
  /* USER CODE BEGIN CompensationTask */
	osEvent evt;
    StatusData_t status;
  /* Infinite loop */
  for(;;)
  {
	  evt = osMessageGet(CompQueueHandle, osWaitForever);
	         if (evt.status == osEventMessage) {
	        	 StatusData_t *statusPtr = (StatusData_t *)evt.value.p;
	        	 status = *statusPtr;
	        	 status.compensatedGas = status.remainingGas * (1.0f - (status.humidity * 0.001f));
	         }
  }

  /* USER CODE END CompensationTask */
}

/* USER CODE BEGIN Header_lcd */
/**
* @brief Function implementing the myTask07 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lcd */
void lcd(void const * argument)
{
  /* USER CODE BEGIN lcd */
	osEvent evt;
	osEvent evtSensor;
	StatusData_t status;
	SensorData_t sensor;
	const char* leakTypeStr[] = {"Normal", "Slow", "Sudden"};
	char line1[17];
	char line2[17];
	lcd_clear();
	lcd_set_cursor(0,0);
	lcd_send_string("System Started");
	osDelay(2000);
  /* Infinite loop */
  for(;;)
  {
	          evt = osMessageGet(LCDQueueHandle,1000);
	          if (evt.status == osEventMessage) {
	          StatusData_t *statusPtr = (StatusData_t *)evt.value.p;
	          status=*statusPtr;
	          }
	          evtSensor=osMessageGet(SensorQueueHandle,0);
	          if(evtSensor.status==osEventMessage){
	          SensorData_t *sensorPtr=(SensorData_t *)evtSensor.value.p;
	          sensor=*sensorPtr;
	          }
	          snprintf(line1, sizeof(line1), "W:%.1fkg T:%.1fC", status.weight, status.temperature);
	          snprintf(line2, sizeof(line2), "L:%s H:%.1f%%", leakTypeStr[status.leakType], status.humidity);
	          lcd_clear();
	          lcd_set_cursor(0,0);
	          lcd_send_string(line1);
	          lcd_set_cursor(1,0);
	          lcd_send_string(line2);
              osDelay(2000);

     }

  /* USER CODE END lcd */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
