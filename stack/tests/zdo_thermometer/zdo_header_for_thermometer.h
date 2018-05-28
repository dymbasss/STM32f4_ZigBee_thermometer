#ifndef __ZB_HEADER_FOR_THERMOMETER_H
#define __ZB_HEADER_FOR_THERMOMETER_H

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_tim.h"

#define B_MAIN  GPIO_Pin_0

void set_send_temperature(zb_callback_t func);
zb_uint16_t set_value_temperature(void);

void init_adc(void);
void init_timer(void);
void init_button(void);
void init_led(void);

typedef enum
  {
    TIM_CMD_DISABLE = 0,
    TIM_CMD_ENABLE
  } tim_cmd;

#endif // !ZB_HEADER_FOR_THERMOMETER_H
