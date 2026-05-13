/*
 * mq7_sensor.c
 *
 *  Created on: May 11, 2026
 *      Author: DELL
 */
#include "mq7_sensor.h"
#include "mq_common.h"
#include <math.h>
#include <stdio.h>

static float MQ7_R0 = 10.0f;
static uint8_t heater_phase = 0; // 0=HIGH, 1=LOW

uint32_t MQ7_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_5); // adjust pin
}

float MQ7_GetPPM(uint32_t adc_val) {
    if (heater_phase != 1) return 0.0f; // only valid in LOW phase

    float v_out = (adc_val / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    if (rs <= 0.0f || MQ7_R0 <= 0.0f) return 0.0f;

    float ratio = rs / MQ7_R0;
    float m = -0.77f, b = 1.7f; // CO curve
    return pow(10, ((log10(ratio) - b) / m));
}

float MQ7_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ7_ReadADC();
        HAL_Delay(50);
    }
    uint32_t avg_adc = sum / 100;

    float v_out = (avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ7_RL_VALUE) / v_sensor;
    MQ7_R0 = rs / 1.5f; // clean-air ratio
    printf("MQ7 ADC=%lu, v_out=%.3f V, v_sensor=%.3f V, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, v_out, v_sensor, rs, MQ7_R0);
    return MQ7_R0;
}

void MQ7_HeaterCycle(void) {
    printf("MQ7 Heater HIGH phase...\r\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    heater_phase = 0;
    HAL_Delay(60000);

    printf("MQ7 Heater LOW phase...\r\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    heater_phase = 1;
    HAL_Delay(90000);

    printf("MQ7 Heater cycle complete, ready to read ADC.\r\n");
}
