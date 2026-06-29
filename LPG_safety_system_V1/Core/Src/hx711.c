/*
 * hx711.c
 *
 *  Created on: May 25, 2026
 *      Author: Rithika
 */

/*  This HX711 driver works by manually implementing the chip’s custom synchronous protocol through bit banging.
 *  At initialization, PB1 (SCK) is set low to establish a stable starting state.
 *   During a read, the STM32 first waits until HX711 signals data ready by pulling PB0 (DOUT) low, then generates 24 clock pulses on PB1;
 *   each rising edge causes HX711 to shift out one bit of its 24 bit ADC register, which the code assembles into value.
 *   After the 24th bit, the code issues one extra pulse (25th), which is not part of the data but re selects the next conversion mode —
 *   in this case Channel A with Gain 128, the default configuration. Because HX711 outputs a 24 bit two’s complement signed value,
 *   the MCU must sign extend it into 32 bits so negative readings are preserved correctly. The tare routine averages multiple samples to
 *   establish a baseline offset, while the weight function subtracts this offset from new raw readings and multiplies by a calibration
 *   factor to convert counts into kilograms. In essence, the code handles clock generation, protocol framing, signed data conversion, and
 *   calibration, turning the milli-volt signal from the load cell into a stable, meaningful weight measurement.*/



#include "hx711.h"
#include "stm32f4xx_hal.h"
#include<stdio.h>

// -------------------- Microsecond delay --------------------
void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

// -------------------- Initialization --------------------
void HX711_Init(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // PB1 = SCK low

}

// -------------------- Read raw 24-bit value --------------------
//read and tare functions commented for debugging purpose
long HX711_Read(void)
{
    return 1000;
}

long HX711_Tare(int samples)
{
    return 0;
}

/*
long HX711_Read(void) {
    long value = 0;

    // Wait until DOUT goes low (data ready)
   // while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0));
    uint32_t start = HAL_GetTick();

    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0))
    {
        if(HAL_GetTick() - start > 2000)
        {
            printf("DOUT TIMEOUT\r\n");
            return 0;
        }
    }

    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        delay_us(10);

        value = (value << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        delay_us(10);
    }

    // 25th pulse for channel A, gain = 128
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    delay_us(10);

    // Sign extension from 24-bit to 32-bit
    if (value & 0x800000) {
        value |= 0xFF000000;
    }

    return value;
}

// -------------------- Tare (zero offset) --------------------
long HX711_Tare(int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += HX711_Read();
        HAL_Delay(10); // ms delay between samples
    }
    return sum / samples;
}
*/
// -------------------- Get weight --------------------
float HX711_GetWeight(long offset, float factor) {
    long raw = HX711_Read(); // The value returned by the function HX711_Read() is assigned to raw.

    long net = raw - offset;
    return net * factor;
}
