/*
 * LCD.c
 *
 *  Created on: Jul 8, 2026
 *      Author: Rithika
 */

#include "lcd.h"
#include<stdint.h>

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);
    data_t[0] = data_u|0x0C;  // en=1, rs=0
    data_t[1] = data_u|0x08;  // en=0, rs=0
    data_t[2] = data_l|0x0C;  // en=1, rs=0
    data_t[3] = data_l|0x08;  // en=0, rs=0
    if( HAL_I2C_Master_Transmit(&hi2c2, LCD_ADDR, data_t, 4, 100));
    {
           // Handle error
           return HAL_ERROR;
       }
       return HAL_OK;
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);
    data_t[0] = data_u|0x0D;  // en=1, rs=1
    data_t[1] = data_u|0x09;  // en=0, rs=1
    data_t[2] = data_l|0x0D;  // en=1, rs=1
    data_t[3] = data_l|0x09;  // en=0, rs=1
    HAL_I2C_Master_Transmit(&hi2c2, LCD_ADDR, data_t, 4, 100);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_set_cursor(int row, int col) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_send_string(char *str) {
    while(*str) lcd_send_data(*str++);
}

void lcd_init(void) {
    HAL_Delay(50); // wait for LCD power up
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20); // 4-bit mode
    HAL_Delay(10);

    lcd_send_cmd(0x28); // 2 line, 5x8 matrix
    lcd_send_cmd(0x08); // display off
    lcd_send_cmd(0x01); // clear display
    HAL_Delay(2);
    lcd_send_cmd(0x06); // entry mode
    lcd_send_cmd(0x0C); // display on, cursor off

}

