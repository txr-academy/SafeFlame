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
static float MQ7_R0 = 1.0f;  // baseline resistance
/* The MQ7 gas sensor has a unique feature compared to MQ2 and MQ4 gas sensor, has a
 *  separate heater cycle routine at 5V and 1.4 where it remains for 60 S and 90 S
 *  respectively.Foe reading valid data we have to read ADC channel during  heater low
 *  phase. But here,MQ7 is powered from a constant 5V DC supply due to which the heater
 *   cycle operation will not work.Hence ADC could be read like MQ2 and MQ4 gas sensors */
uint32_t MQ7_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_5); // PA5 connected to AO
}

float MQ7_GetPPM(uint32_t adc_val) {
    float v_out = ((float)adc_val / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    if (rs <= 0.0f || MQ7_R0 <= 0.0f) return 0.0f;

    float ratio = rs / MQ7_R0;
    float m = -0.77f, b = 1.7f; // CO curve approximation
    return powf(10.0f, ((log10f(ratio) - b) / m));
}

float MQ7_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ7_ReadADC();
        HAL_Delay(50);
    }
    uint32_t avg_adc = sum / 100;

    float v_out = ((float)avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    MQ7_R0 = rs / 1.5f; // clean-air ratio
    printf("MQ7 Calibrated: ADC=%lu, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, rs, MQ7_R0);
    return MQ7_R0;
}
