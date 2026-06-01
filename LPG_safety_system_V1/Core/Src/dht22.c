/*
 * dht22.c
 *
 *  Created on: May 25, 2026
 *      Author: Rithika
 */

/*This DHT22 driver implements the sensor’s single wire protocol with precise timing control.
 * The microsecond delay function uses the STM32’s DWT cycle counter to generate accurate delays,
 * while DHT22_WaitForPin ensures transitions occur within a timeout so the MCU never hangs.
 * To start communication, the MCU drives the data pin low for 20 ms, then releases it and switches the pin to input mode;
 *  the DHT22 responds with a specific low high handshake. The main read loop then captures 40 bits: each bit begins with
 *  a low pulse, followed by a high pulse whose duration encodes 0 or 1. The code samples the line after ~40 µs to decide
 *  the bit value and assembles the result into five bytes. A checksum verifies data integrity, and the raw humidity and
 *  temperature values are extracted from the first four bytes, scaled by 10 to yield human readable floats. In essence,
 *  the code handles clock like timing generation, protocol framing, error detection, and conversion, turning the DHT22’s
 *   pulse width encoded signal into stable temperature and humidity readings. */

#include "dht22.h"

/* Microsecond delay using DWT cycle counter.HAL_Delay() cannot be used because,DHT22 requires microsecond level operation.HAL delay can only generate ms delay.
 * The DWT (Data Watchpoint and Trace) cycle counter is a hardware feature on ARM Cortex-M processors (like Cortex-M3, M4, M7) used for high-precision,
 * sub-microsecond code profiling. It features a 32-bit free-running register (DWT_CYCCNT) that increments on every CPU clock cycle. */

//Function definition for microsecond delay
static void delay_us(uint32_t us) {
    uint32_t cycles = (SystemCoreClock / 1000000) * us; //SystemClock=168 MHz, us is the parameter passed by the function delay_us();
    uint32_t start = DWT->CYCCNT; //Records current cycle count.Then it loops until the difference between the current cycle count and start reaches 168*us cycles, which gives the exact delay.
    while ((DWT->CYCCNT - start) < cycles);
}

// Helper: wait for pin to reach state with timeout
static uint8_t DHT22_WaitForPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                                GPIO_PinState state, uint32_t timeout_us) {
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) != state) {
        if (timeout_us-- == 0) return 0; // timeout
        delay_us(1);
    }
    return 1; // success
}

// Switch pin to output
static void DHT22_SetPinOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

// Switch pin to input with pull‑up
static void DHT22_SetPinInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

// Main read function
HAL_StatusTypeDef DHT22_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                             float *temperature, float *humidity) {
    uint8_t data[5] = {0};

    // 1. Send start signal
    DHT22_SetPinOutput(GPIOx, GPIO_Pin);
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    HAL_Delay(20); // 20 ms low
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    delay_us(30);
    DHT22_SetPinInput(GPIOx, GPIO_Pin);

    // 2. Wait for sensor response with timeout
    if (!DHT22_WaitForPin(GPIOx, GPIO_Pin, GPIO_PIN_RESET, 100)) return HAL_ERROR;
    if (!DHT22_WaitForPin(GPIOx, GPIO_Pin, GPIO_PIN_SET, 100)) return HAL_ERROR;

    // 3. Read 40 bits
    for (int i = 0; i < 40; i++) {
        if (!DHT22_WaitForPin(GPIOx, GPIO_Pin, GPIO_PIN_RESET, 100)) return HAL_ERROR;
        if (!DHT22_WaitForPin(GPIOx, GPIO_Pin, GPIO_PIN_SET, 100)) return HAL_ERROR;

        delay_us(40);
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))
            data[i/8] |= (1 << (7 - (i % 8)));

        if (!DHT22_WaitForPin(GPIOx, GPIO_Pin, GPIO_PIN_RESET, 100)) return HAL_ERROR;
    }

    // 4. Verify checksum
    if ((data[0] + data[1] + data[2] + data[3]) != data[4]) return HAL_ERROR;

    // 5. Convert to values
    uint16_t raw_hum = (data[0] << 8) | data[1];
    uint16_t raw_temp = (data[2] << 8) | data[3];
    *humidity = raw_hum / 10.0f;
    *temperature = raw_temp / 10.0f;

    return HAL_OK;
}
