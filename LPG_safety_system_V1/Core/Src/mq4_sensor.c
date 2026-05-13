/*
 * mq4_sensor.c
 *
 *  Created on: May 11, 2026
 *      Author: DELL
 */
#include "mq4_sensor.h"
#include "mq_common.h"
#include <math.h>
#include <stdio.h>

// Adjust RL to match your MQ4 module (often 20kΩ)
#define MQ4_SUPPLY_VOLTAGE 5.0f
#define MQ4_RL_VALUE 10.0f   // kΩ

static float MQ4_R0 = 10.0f;

uint32_t MQ4_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_12); // use channel 12 as you wired
}

float MQ4_GetPPM(uint32_t adc_val) {
    float v_out = (adc_val / ADC_RES) * VREF;        // voltage at STM32 pin
    float v_sensor = v_out * DIVIDER_GAIN;           // reconstruct sensor voltage
    if (v_sensor > MQ4_SUPPLY_VOLTAGE) v_sensor = MQ4_SUPPLY_VOLTAGE; // clamp at 5V

    float rs = ((MQ4_SUPPLY_VOLTAGE - v_sensor) * MQ4_RL_VALUE) / v_sensor;
    if (rs <= 0.0f || MQ4_R0 <= 0.0f) return 0.0f;

    float ratio = rs / MQ4_R0;
    float m = -0.38f, b = 1.3f; // CH4 curve constants
    return pow(10, ((log10(ratio) - b) / m));
}

float MQ4_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ4_ReadADC();
        HAL_Delay(50);
    }
    uint32_t avg_adc = sum / 100;

    float v_out = (avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > MQ4_SUPPLY_VOLTAGE) v_sensor = MQ4_SUPPLY_VOLTAGE;

    float rs = ((MQ4_SUPPLY_VOLTAGE - v_sensor) * MQ4_RL_VALUE) / v_sensor;
    MQ4_R0 = rs / 4.4f; // clean-air ratio for MQ4
    printf("MQ4 ADC=%lu, v_out=%.3f V, v_sensor=%.3f V, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, v_out, v_sensor, rs, MQ4_R0);
    return MQ4_R0;
}
