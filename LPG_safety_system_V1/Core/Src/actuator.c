/*
 * actuator.c
 *
 *  Created on: Jun 17, 2026
 *      Author: Rithika
 */
#include "actuator.h"

// To be configured
void soundBuzzer(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // turn buzzer ON
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // turn buzzer OFF
}

// To be configured
void closeValve(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // drive valve closed
}


