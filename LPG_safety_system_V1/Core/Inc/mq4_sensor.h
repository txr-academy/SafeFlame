/*
 * mq4_sensor.h
 *
 *  Created on: May 11, 2026
 *      Author: Rithika
 */

#ifndef MQ4_SENSOR_H
#define MQ4_SENSOR_H

#include "main.h"
#define DIVIDER_GAIN   1.0f   // breakout already scales to 3.3V
#define MQ4_RL_VALUE   5.0f   // Load resistor (kΩ)

uint32_t MQ4_ReadADC(void);
float MQ4_GetPPM(uint32_t adc_val);
float MQ4_CalibrateR0(void);

#endif
