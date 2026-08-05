/*
 * gsm.h
 *
 * GSM driver header for STM32 + SIM800L
 * Author: Rithika
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
extern osMessageQId GSMQueueHandle;


// Initialize GSM module (disable echo, basic setup)
void GSM_Init(void);

// Send a generic AT command and print response
void GSM_SendCommand(const char *cmd);

// Send an SMS to the given number with the given message
void GSM_SendSMS(const char *number, const char *message);

#endif /* INC_GSM_H_ */
