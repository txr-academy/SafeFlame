/*
 * dht22.h
 *
 *  Created on: May 25, 2026
 *      Author: DELL
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_


#include "stm32f4xx_hal.h"

HAL_StatusTypeDef DHT22_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                             float *temperature, float *humidity);


#endif /* INC_DHT22_H_ */
