/*
 * GSM.h
 *
 *  Created on: Jul 23, 2026
 *      Author:Rithika
 */
#ifndef __GSM_H
#define __GSM_H
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

// Queue handle for GSM task
extern osMessageQId GSMQueueHandle;
void GSM_SendSMS(char *number, char *message);

#endif /* __GSM_H */


