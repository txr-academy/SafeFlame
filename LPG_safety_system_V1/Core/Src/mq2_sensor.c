/*
 * mq2_sensor.c
 *
 *  Created on: May 11, 2026
 *      Author: Rithika
 */

#include "mq2_sensor.h"
#include "mq_common.h"
#include <math.h>
#include <stdio.h>

/*This MQ‑2 driver code is designed to convert raw ADC readings into meaningful gas concentration values. The sensor’s analog output is sampled by the STM32’s 12‑bit ADC
 *  (resolution = 4096 steps, with reference voltage VREF = 3.3 V),  producing a digital value that is scaled back into a voltage (v_out). Because the breakout board uses
 *   a resistor divider, the actual sensor voltage (v_sensor) is reconstructed by multiplying with the divider gain (e.g., 11 for a 10 kΩ/1 kΩ divider).
 *  From this voltage, the sensor resistance is calculated using the load resistor value (MQ2_RL_VALUE, typically 10 kΩ) and the voltage divider formula.
 *  To normalize readings, is divided by the baseline resistance R0 which is established during calibration in clean air by averaging 100 ADC samples and applying the
 *  datasheet’s clean air ratio (~9.8). Finally, the ratio Rs/R0 is mapped to ppm using a logarithmic curve defined by slope (m = -0.45) and intercept (b = 1.0) constants
 *   derived from the MQ‑2 datasheet for LPG. This pipeline—ADC scaling, sensor voltage reconstruction, resistance calculation, calibration of R0 and logarithmic ppm conversion
 *   —ensures accurate and stable gas concentration measurements. */

static float MQ2_R0 = 10.0f;

uint32_t MQ2_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_10); // adjust pin accordingly
}

float MQ2_GetPPM(uint32_t adc_val) {
    float v_out = (adc_val / ADC_RES) * VREF;//
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ2_RL_VALUE) / v_sensor;
    if (rs <= 0.0f || MQ2_R0 <= 0.0f) return 0.0f;

    float ratio = rs / MQ2_R0;
    float m = -0.45f, b = 1.0f; // LPG curve
    return pow(10, ((log10(ratio) - b) / m));
}

float MQ2_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ2_ReadADC();
        HAL_Delay(50);
    }
    uint32_t avg_adc = sum / 100;

    float v_out = (avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * DIVIDER_GAIN;
    if (v_sensor > VREF) v_sensor = VREF;

    float rs = ((VREF - v_sensor) * MQ2_RL_VALUE) / v_sensor;
    MQ2_R0 = rs / 9.8f; // clean-air ratio
    printf("MQ2 ADC=%lu, v_out=%.3f V, v_sensor=%.3f V, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, v_out, v_sensor, rs, MQ2_R0);
    return MQ2_R0;
}
