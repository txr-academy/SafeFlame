/*
 * mq7_sensor.h
 *
 *  Created on: May 11, 2026
 *      Author: DELL
 */

#ifndef MQ7_SENSOR_H
#define MQ7_SENSOR_H
#include "main.h"


uint32_t MQ7_ReadADC(void);
float MQ7_GetPPM(uint32_t adc_val);
float MQ7_CalibrateR0(void);


#endif
