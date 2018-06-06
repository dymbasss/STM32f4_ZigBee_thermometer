#ifndef __ZB_HEADER_FOR_BUTTON_H
#define __ZB_HEADER_FOR_BUTTON_H

#include "zdo_header_for_lcd.h"
#include "stm32f4xx_tim.h"

#define B_MAIN  GPIO_Pin_0

void set_request_for_send_temperature(zb_callback_t);

void init_button(void);

#endif // !ZB_HEADER_FOR_LCD_H
