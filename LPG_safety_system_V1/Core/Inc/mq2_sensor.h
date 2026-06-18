/*
 * mq2_sensor.h
 *
 *  Created on: May 11, 2026
 *      Author:Rithika
 */

#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H
#include <stdint.h>

// Adjust these constants to match your hardware setup
#define ADC_RES        4096.0f   // 12-bit ADC resolution
#define VREF           3.3f      // STM32 ADC reference voltage
#define DIVIDER_GAIN   1.0f      // Use 1.0 if breakout already scales to 3.3V
#define MQ2_RL_VALUE   5.0f     // Load resistor in kΩ (check your module)

// Function prototypes
void     MQ2_Init(void);             // Optional init (can be empty)
uint32_t MQ2_ReadADC(void);          // Read raw ADC value
float    MQ2_CalibrateR0(void);      // Calibrate baseline R0 in clean air
float    MQ2_GetPPM(uint32_t adc_val); // Convert ADC reading to ppm

#endif /* MQ2_SENSOR_H */



