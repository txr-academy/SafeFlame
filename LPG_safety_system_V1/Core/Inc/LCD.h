/*
 * LCD.h
 *
 *  Created on: Jul 8, 2026
 *      Author: Rithika
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "stm32f4xx_hal.h"
#define LCD_ADDR 0x27 << 1   // I2C slave address for the used LCD module is 0x27.
extern I2C_HandleTypeDef hi2c2; // use the configured I2C handle in the main function

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_set_cursor(int row, int col);

#endif /* INC_LCD_H_ */
