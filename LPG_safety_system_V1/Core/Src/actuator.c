/*
 * actuator.c
 *
 *  Created on: Jun 17, 2026
 *      Author: Rithika
 */
#include "actuator.h"

/*The selected buzzer module is active low,hence the GPIO line is first RESET then SET.
 * In the ioc file the gpio line is configured as no pull up or pull down and output low.
 * Again in the main.c code configure gpio line as SET (by default it will be RESET state)
 */
void soundBuzzer(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // turn buzzer ON
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // turn buzzer OFF
    HAL_Delay(1000);
}

/* TBD
void closeValve(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // drive valve closed
}

*/
