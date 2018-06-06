#ifndef __ZB_HEADER_FOR_LCD_H
#define __ZB_HEADER_FOR_LCD_H 

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#include "stm32f4xx_i2c.h"
#include "stm32f4xx_tim.h"

#define COMMAND_UPDATE_TEMPERATURE 0x54
#define COMMAND_REQUEST_UPDATE_TEMPERATURE 0x55
#define DATA_BYTE 1

#define LCD_ADDR  0x07 // Addres
#define LCD_1602 // Type LCD

#define PCF_P0  0
#define PCF_P1	1
#define PCF_P2	2
#define PCF_P3	3
#define PCF_P4	4
#define PCF_P5	5
#define PCF_P6	6
#define PCF_P7	7

// Pins LCD
#define DB4  PCF_P4
#define DB5  PCF_P5
#define DB6  PCF_P6
#define DB7  PCF_P7
#define RS   PCF_P0
#define RW   PCF_P1
#define EN   PCF_P2
#define BL   PCF_P3

extern void delay_ms(zb_uint16_t);
#define lcd_pause  delay_ms(1000)

void lcd_init(void);
void lcd_backlight(zb_uint8_t); // On/off backlight
void lcd_goto(zb_uint8_t, zb_uint8_t); // Transition line / line item 
void lcd_print(zb_uint8_t *); // Write string 

void lcd_send(zb_uint8_t); // send value in LCD
void lcd_command(zb_uint8_t); // send command in LCD
void lcd_data(zb_uint8_t); // send data in LCD

void init_pin(void);
void init_i2c(void);

#endif // !ZB_HEADER_FOR_LCD_H
