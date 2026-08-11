/*
 * gsm.c
 *
 * GSM driver for STM32 + SIM800L
 * Author: Rithika
 */
#include "gsm.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include"status.h"

extern UART_HandleTypeDef huart2;   // SIM800L UART
extern UART_HandleTypeDef huart3;   // Debug UART
#define GSM_RX_BUFFER_SIZE    128
static uint8_t gsm_rx_buffer[GSM_RX_BUFFER_SIZE];
static void GSM_SendRaw(const char *cmd)
{
    HAL_UART_Transmit(&huart2,(uint8_t *)cmd, strlen(cmd), 1000);
}
static uint16_t GSM_ReadResponse(uint32_t timeout)
{
    uint8_t ch;
    uint16_t index = 0;
    uint32_t start_time;

    memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));

    start_time = HAL_GetTick();

    printf("\r\nGSM Response: ");

    while ((HAL_GetTick() - start_time) < timeout)
    {
        if (HAL_UART_Receive(&huart2, &ch, 1, 100) == HAL_OK)
        {
            if (index < GSM_RX_BUFFER_SIZE - 1)
            {
                gsm_rx_buffer[index++] = ch;
                gsm_rx_buffer[index] = '\0';
            }
            HAL_UART_Transmit(&huart3, &ch, 1, 100);
            if (strstr((char *)gsm_rx_buffer, "OK") != NULL)
            {
                break;
            }
            if (strstr((char *)gsm_rx_buffer, "ERROR") != NULL)
            {
                break;
            }
        }
    }

    printf("\r\n");

    return index;
}
void GSM_Init(void)
{
    const char *msg;

   // msg = "\r\n========== GSM INIT ==========\r\n";
    HAL_UART_Transmit(&huart3,(uint8_t *)msg,strlen(msg),1000);
    GSM_SendRaw("ATE0\r\n");

    memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));
    GSM_ReadResponse(2000);
    osDelay(500);
    GSM_SendRaw("AT+CMGF=1\r\n");

    memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));
    GSM_ReadResponse(2000);
    osDelay(500);
    GSM_SendRaw("AT+CSCS=\"GSM\"\r\n");

    memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));
    GSM_ReadResponse(2000);
    osDelay(500);

    GSM_SendRaw("AT+CREG?\r\n");

    memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));
    GSM_ReadResponse(3000);

    //msg = "\r\n========== GSM INIT DONE ==========\r\n";

    HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
}

void GSM_SendCommand(const char *cmd)
{
 GSM_SendRaw(cmd);
 memset(gsm_rx_buffer, 0, sizeof(gsm_rx_buffer));
 GSM_ReadResponse(3000);
}

void GSM_SendSMS(const char *number, const char *message)
{
    char cmd[64];
    uint8_t ch;
    uint8_t ctrlZ = 0x1A;
    uint8_t prompt_received = 0;
   printf("\r\n========== SMS START ==========\r\n");
   while (HAL_UART_Receive(&huart2, &ch, 1, 10) == HAL_OK)
    {

    }
    printf("Sending AT+CMGF=1...\r\n");

    GSM_SendRaw("AT+CMGF=1\r\n");

    GSM_ReadResponse(3000);

    osDelay(1000);
    while (HAL_UART_Receive(&huart2, &ch, 1, 10) == HAL_OK)
    {

    }

    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"\r", number);
    printf("Sending: %s\r\n", cmd);
       GSM_SendRaw(cmd);
    printf("Waiting for SMS prompt '>'...\r\n");

    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < 10000)
    {
        if (HAL_UART_Receive(&huart2,&ch,1,100) == HAL_OK)
        {

            HAL_UART_Transmit(&huart3, &ch, 1,100);
            if (ch == '>')
            {
                prompt_received = 1;
                break;
            }
        }
    }
    if (!prompt_received)
    {
        printf("\r\nERROR: No '>' prompt from SIM800L\r\n");
        printf("SMS NOT SENT\r\n");
        printf("========== SMS END ==========\r\n");
        return;
    }

    printf("\r\nSMS prompt received!\r\n");


    printf("Sending SMS text...\r\n");

    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), 5000);
    osDelay(200);



    printf("Sending CTRL+Z...\r\n");

    HAL_UART_Transmit(&huart2,&ctrlZ,1,1000);
    printf("Waiting for SMS confirmation...\r\n");
    GSM_ReadResponse(1000);
    printf("\r\n========== SMS END ==========\r\n");
}
void GSM_SendLeakAlert(StatusData_t *status)
{
    char msg[160];
    const char *leakTypeStr[] =
    {
        "Normal",
        "Slow",
        "Sudden"
    };
    const char *gasTypeStr;
    if (status->anomalyFlag == 1)
    {
        gasTypeStr = "LPG/Methane";
    }
    else if (status->anomalyFlag == 2)
    {
        gasTypeStr = "CO";
    }
    else
    {
        gasTypeStr = "Unknown";
    }

    uint8_t leakType = status->leakType;

    if (leakType > LEAK_SUDDEN)
    {
        leakType = LEAK_NORMAL;
    }
   //snprintf(msg,  sizeof(msg),  "ALERT: Leak=%s", leakTypeStr[leakType]);//Added for debugging
  snprintf(msg,  sizeof(msg),  "ALERT: Leak=%s, Gas=%s\r\n"    "MQ2=%.1f ppm\r\n"  "MQ4=%.1f ppm\r\n"   "MQ7=%.1f ppm",
  leakTypeStr[leakType],  gasTypeStr,  status->mq2_ppm,    status->mq4_ppm,   status->mq7_ppm);

    printf("\r\nSMS CONTENT:\r\n");
    printf("%s\r\n", msg);

    GSM_SendSMS("+919207436470", msg);
}

