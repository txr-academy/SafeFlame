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
#include "stm32f4xx_hal.h"

/*This MQ‑2 driver code is designed to convert raw ADC readings into meaningful gas concentration values. The sensor’s analog output is sampled by the STM32’s 12‑bit ADC
 *  (resolution = 4096 steps, with reference voltage VREF = 3.3 V),  producing a digital value that is scaled back into a voltage (v_out). Because the breakout board uses
 *   a resistor divider, the actual sensor voltage (v_sensor) is reconstructed by multiplying with the divider gain (e.g., 11 for a 10 kΩ/1 kΩ divider).
 *  From this voltage, the sensor resistance is calculated using the load resistor value (MQ2_RL_VALUE, typically 10 kΩ) and the voltage divider formula.
 *  To normalize readings, is divided by the baseline resistance R0 which is established during calibration in clean air by averaging 100 ADC samples and applying the
 *  datasheet’s clean air ratio (~9.8). Finally, the ratio Rs/R0 is mapped to ppm using a logarithmic curve defined by slope (m = -0.45) and intercept (b = 1.0) constants
 *   derived from the MQ‑2 datasheet for LPG. This pipeline—ADC scaling, sensor voltage reconstruction, resistance calculation, calibration of R0 and logarithmic ppm conversion
 *   —ensures accurate and stable gas concentration measurements. */

/*
 * mq2_sensor.c
 * Independent MQ-2 driver for STM32 HAL
 * Author: Rithika
 */
#define MQ2_SUPPLY_VOLTAGE 3.3f

static float MQ2_R0 = 1.0f;
uint32_t MQ2_ReadADC(void) {
    return Read_ADC_Channel(ADC_CHANNEL_10);
}
float MQ2_GetPPM(uint32_t adc_val) {
    float v_out = ((float)adc_val / ADC_RES) * VREF;
    float v_sensor = v_out * 1.0f; // no divider gain


    if (v_sensor <= 0.01f || MQ2_R0 <= 0.0f) return 0.0f;

    float rs = ((MQ2_SUPPLY_VOLTAGE - v_sensor) * MQ2_RL_VALUE) / v_sensor;
    float ratio = rs / MQ2_R0;

    // Clamp ratio
    if (ratio < 0.1f) ratio = 0.1f;
    if (ratio > 10.0f) ratio = 10.0f;

    // LPG curve constants
    float m = -0.45f, b = 1.0f;
    return powf(10.0f, ((log10f(ratio) - b) / m));
    float ppm_mq2 = MQ2_GetPPM(adc_val);
    printf("MQ2 PPM = %.2f\r\n", ppm_mq2);

}

float MQ2_CalibrateR0(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += MQ2_ReadADC();
        HAL_Delay(50);
    }
    uint32_t avg_adc = sum / 100;

    float v_out = ((float)avg_adc / ADC_RES) * VREF;
    float v_sensor = v_out * 1.0f;

    if (v_sensor <= 0.01f) {
        MQ2_R0 = 0.0f;
        return MQ2_R0;
    }

    float rs = ((MQ2_SUPPLY_VOLTAGE - v_sensor) * MQ2_RL_VALUE) / v_sensor;
    MQ2_R0 = rs / 9.8f; // clean-air ratio for MQ2

    printf("MQ2 Calibrated: ADC=%lu, v_out=%.3f V, v_sensor=%.3f V, Rs=%.4f kΩ, R0=%.4f kΩ\r\n",
           avg_adc, v_out, v_sensor, rs, MQ2_R0);

    return MQ2_R0;
}
