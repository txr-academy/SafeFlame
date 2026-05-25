/*
 * hx711.h
 *
 *  Created on: May 25, 2026
 *      Author: DELL
 */

#ifndef INC_HX711_H_
#define INC_HX711_H_
#define SCALE_FACTOR 1000.0f   // replace 1000.0f with your calibrated value


#include "stm32f4xx_hal.h"

typedef struct {
    GPIO_TypeDef* dt_port;
    uint16_t dt_pin;
    GPIO_TypeDef* sck_port;
    uint16_t sck_pin;
} HX711;

void HX711_Init(HX711* hx711, GPIO_TypeDef* dt_port, uint16_t dt_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin);
int32_t HX711_Read(HX711* hx711);
void HX711_Tare(HX711* hx711, uint8_t times);
float HX711_GetWeight(HX711* hx711, float scale);


#endif /* INC_HX711_H_ */
