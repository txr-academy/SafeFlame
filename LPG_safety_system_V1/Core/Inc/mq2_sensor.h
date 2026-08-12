/*
 * mq2_sensor.h
 *
 *  Created on: May 11, 2026
 *      Author:Rithika
 */

#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H
#include <stdint.h>

void     MQ2_Init(void);             // Optional init (can be empty)
uint32_t MQ2_ReadADC(void);          // Read raw ADC value
float    MQ2_CalibrateR0(void);      // Calibrate baseline R0 in clean air
float    MQ2_GetPPM(uint32_t adc_val); // Convert ADC reading to ppm

#endif /* MQ2_SENSOR_H */



