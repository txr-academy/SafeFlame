/*
 * mq7_sensor.c
 *
 *  Created on: May 11, 2026
 *      Author: Rithika
 */
#include "mq7_sensor.h"
#include "mq_common.h"
#include <math.h>
#include <stdio.h>

static float MQ7_R0 = 10.0f;  // baseline resistance

uint32_t MQ7_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_5); // PA5 connected to AO
}

float MQ7_GetPPM(uint32_t adc_val) {
    float v_out = (adc_val / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    if (rs <= 0.0f || MQ7_R0 <= 0.0f) return 0.0f;

    float ratio = rs / MQ7_R0;
    float m = -0.77f, b = 1.7f; // CO curve approximation
    return pow(10, ((log10(ratio) - b) / m));
}

float MQ7_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ7_ReadADC();
        HAL_Delay(50); // okay during calibration only
    }
    uint32_t avg_adc = sum / 100;

    float v_out = (avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    MQ7_R0 = rs / 1.5f; // clean-air ratio
    printf("MQ7 Calibrated: ADC=%lu, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, rs, MQ7_R0);
    return MQ7_R0;
}
