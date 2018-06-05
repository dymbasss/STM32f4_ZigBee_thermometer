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

#define COMMAND_UPDATE_TEMPERATURE 0x54 

void set_temperature_for_send(zb_callback_t);
void schedule_callback_update_temperature(zb_uint8_t);
zb_uint8_t value_temperature_broadcast(void);

void init_adc(void);
void init_timer(void);

typedef enum
  {
    TIM_CMD_DISABLE = 0,
    TIM_CMD_ENABLE
  } tim_cmd;

#endif // !ZB_HEADER_FOR_THERMOMETER_H
