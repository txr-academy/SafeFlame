/*
 * mq2_sensor.h
 *
 *  Created on: May 11, 2026
 *      Author: DELL
 */

#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

#include "main.h"

#define MQ2_RL_VALUE   10.0f   // Load resistor (kΩ)

uint32_t MQ2_ReadADC(void);
float MQ2_GetPPM(uint32_t adc_val);
float MQ2_CalibrateR0(void);

#endif





