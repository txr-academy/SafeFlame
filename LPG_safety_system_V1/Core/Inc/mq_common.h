/*
 * mq_common.h
 *
 *  Created on: May 13, 2026
 *      Author: DELL
 */

#ifndef MQ_COMMON_H
#define MQ_COMMON_H

#include <stdint.h>

#define VREF        3.3f
#define ADC_RES     4095.0f
#define DIVIDER_GAIN 11.0f   // 10k + 1k divider

// Load resistors-obtained from respective datasheet
#define MQ2_RL_VALUE 20.0f    // kΩ
#define MQ4_RL_VALUE 20.0f    // kΩ
#define MQ7_RL_VALUE 5.0f   // kΩ
// Supply voltage for each sensor
#define MQ2_SUPPLY_VOLTAGE 5.0f
#define MQ4_SUPPLY_VOLTAGE 5.0f
#define MQ7_SUPPLY_VOLTAGE 5.0f

uint32_t Read_ADC_Channel(uint32_t channel);

#endif
