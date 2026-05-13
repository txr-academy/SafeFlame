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

// Load resistors (measure if different)
#define MQ2_RL_VALUE 10.0f    // kΩ
#define MQ4_RL_VALUE 10.0f    // kΩ
#define MQ7_RL_VALUE 10.0f   // kΩ

uint32_t Read_ADC_Channel(uint32_t channel);

#endif
