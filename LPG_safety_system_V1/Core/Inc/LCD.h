/*
 * LCD.h
 *
 *  Created on: Jul 8, 2026
 *      Author: Rithika
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_


#define LCD_ADDR 0x27 << 1   // I2C address (commonly 0x27 or 0x3F, shift for HAL)

extern I2C_HandleTypeDef hi2c1; // use your I2C handle

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_set_cursor(int row, int col);





#endif /* INC_LCD_H_ */
