/*
 * hx711.c
 *
 *  Created on: May 25, 2026
 *      Author: DELL
 */
#include "hx711.h"

static void HX711_Pulse(HX711* hx711) {
    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

void HX711_Init(HX711* hx711, GPIO_TypeDef* dt_port, uint16_t dt_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin) {
    hx711->dt_port = dt_port;
    hx711->dt_pin = dt_pin;
    hx711->sck_port = sck_port;
    hx711->sck_pin = sck_pin;
}

int32_t HX711_Read(HX711* hx711) {
    while(HAL_GPIO_ReadPin(hx711->dt_port, hx711->dt_pin) == GPIO_PIN_SET);

    int32_t value = 0;
    for(int i=0; i<24; i++) {
        HX711_Pulse(hx711);
        value = (value << 1) | HAL_GPIO_ReadPin(hx711->dt_port, hx711->dt_pin);
    }
    // Set gain = 128 (channel A)
    HX711_Pulse(hx711);

    if(value & 0x800000) value |= ~0xFFFFFF; // sign extend
    return value;
}

void HX711_Tare(HX711* hx711, uint8_t times) {
    int64_t sum = 0;
    for(uint8_t i=0; i<times; i++) {
        sum += HX711_Read(hx711);
    }
    sum /= times;
    // store offset globally or in struct
}

float HX711_GetWeight(HX711* hx711, float scale) {
    int32_t raw = HX711_Read(hx711);
    return (float)raw / scale; // scale factor from calibration
}


