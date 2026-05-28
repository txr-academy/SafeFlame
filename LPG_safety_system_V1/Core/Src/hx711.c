/*
 * hx711.c
 *
 *  Created on: May 25, 2026
 *      Author: DELL
 */
#include "hx711.h"

void HX711_Init(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // PB1 = SCK
}

long HX711_Read(void) {
    long value = 0;
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)); // wait for DT low

    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        value = (value << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    value ^= 0x800000; // signed conversion
    return value;
}

long HX711_Tare(int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += HX711_Read();
    }
    return sum / samples;
}

float HX711_GetWeight(long offset, float factor) {
    long raw = HX711_Read();
    long net = raw - offset;
    return net * factor;
}
