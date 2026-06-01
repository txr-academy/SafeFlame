/*
 * hx711.h
 *
 *  Created on: May 25, 2026
 *      Author: Rithika
 */

#ifndef __HX711_H
#define __HX711_H

#include "stm32f4xx_hal.h"

void HX711_Init(void);
long HX711_Read(void);
long HX711_Tare(int samples);
float HX711_GetWeight(long offset, float factor);

#endif

