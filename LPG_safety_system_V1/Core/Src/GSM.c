/*
 * gsm.c
 *
 * GSM driver for STM32 + SIM800L
 * Author: Rithika
 */

#include "gsm.h"
#include "string.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2; // GSM UART
extern UART_HandleTypeDef huart3; // Debug UART

static uint8_t rxBuf[128];

static void GSM_SendRaw(const char *cmd)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
}

static void GSM_ReadResponse(uint32_t timeout)
{
    memset(rxBuf, 0, sizeof(rxBuf));
    if (HAL_UART_Receive(&huart2, rxBuf, sizeof(rxBuf), timeout) == HAL_OK) {
        // Forward to debug UART
        HAL_UART_Transmit(&huart3, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);

        // Also push into GSMQueue for CommunicationTask
        osMessagePut(GSMQueueHandle, (uint32_t)rxBuf, 0);
    } else {
        const char *msg = "No GSM reply\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        osMessagePut(GSMQueueHandle, (uint32_t)msg, 0);
    }
}

void GSM_Init(void)
{
    GSM_SendRaw("ATE0\r\n");   // Disable echo
    GSM_ReadResponse(2000);
}

void GSM_SendCommand(const char *cmd)
{
    GSM_SendRaw(cmd);
    GSM_ReadResponse(2000);
}

void GSM_SendSMS(const char *number, const char *message)
{
    char cmd[64];
    uint8_t ctrlZ = 0x1A;

    // Set SMS text mode
    GSM_SendCommand("AT+CMGF=1\r\n");
    HAL_Delay(500);

    // Send recipient number
    sprintf(cmd, "AT+CMGS=\"%s\"\r\n", number);
    GSM_SendRaw(cmd);

    // Wait for '>' prompt
    uint8_t ch;
    while (1) {
        if (HAL_UART_Receive(&huart2, &ch, 1, 5000) == HAL_OK) {
            HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY); // debug
            if (ch == '>') break;
        }
    }

    // Send message body
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);

    // End with Ctrl+Z
    HAL_UART_Transmit(&huart2, &ctrlZ, 1, HAL_MAX_DELAY);

    // Read final response
    GSM_ReadResponse(5000);
}

