#ifndef __ZB_HEADER_FOR_THERMOMETER_H
#define __ZB_HEADER_FOR_THERMOMETER_H

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#include "stm32f4xx_adc.h"

#define COMMAND_UPDATE_TEMPERATURE 0x54
#define COMMAND_REQUEST_UPDATE_TEMPERATURE 0x55

zb_uint8_t value_temperature(void);

void init_adc(void);

#endif // !ZB_HEADER_FOR_THERMOMETER_H
