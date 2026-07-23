/*
 * GSM.c
 *
 *  Created on: Jul 23, 2026
 *      Author: Rithika
 */
#include "GSM.h"
#include "string.h"
#include "stdio.h"
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
void GSM_SendSMS(char *number, char *message)
{
    char cmd[64];
    uint8_t rxBuf[128];

    // Set SMS text mode
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CMGF=1\r\n", 11, HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, rxBuf, sizeof(rxBuf), 1000); // read reply
    HAL_UART_Transmit(&huart3, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY); // echo to PC
    HAL_Delay(500);

    // Send command with phone number
    sprintf(cmd, "AT+CMGS=\"%s\"\r\n", number);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, rxBuf, sizeof(rxBuf), 1000);
    HAL_UART_Transmit(&huart3, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);
    HAL_Delay(500);

    // Send message body
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);

    // End with Ctrl+Z
    uint8_t ctrlZ = 0x1A;
    HAL_UART_Transmit(&huart2, &ctrlZ, 1, HAL_MAX_DELAY);
    // Read final GSM response
    HAL_UART_Receive(&huart2, rxBuf, sizeof(rxBuf), 5000);
    HAL_UART_Transmit(&huart3, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);

    HAL_Delay(5000); // wait for GSM module to finish
}
uint8_t gsmRxBuf[128];



