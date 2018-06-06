#include "LIB_INC/zdo_header_for_button.h"

void schedule_callback_request_temperature(void);

zb_callback_t request_temperature_callback;

//-----------------------------------------------------------------------

void init_button(void)
{
  GPIO_InitTypeDef b_init_struct;
  NVIC_InitTypeDef b_nvic_init_struct;
  EXTI_InitTypeDef b_exti_init_struct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  b_init_struct.GPIO_Pin = B_MAIN;
  b_init_struct.GPIO_Mode = GPIO_Mode_IN;
  b_init_struct.GPIO_OType = GPIO_OType_PP;
  b_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
  b_init_struct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &b_init_struct);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  b_exti_init_struct.EXTI_Line = EXTI_Line0;
  b_exti_init_struct.EXTI_LineCmd = ENABLE;
  b_exti_init_struct.EXTI_Mode = EXTI_Mode_Interrupt;
  b_exti_init_struct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&b_exti_init_struct);
	    
  b_nvic_init_struct.NVIC_IRQChannel = EXTI0_IRQn;
  b_nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0x00;
  b_nvic_init_struct.NVIC_IRQChannelSubPriority = 0x00;
  b_nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&b_nvic_init_struct);
}

void EXTI0_IRQHandler(void)
{
  zb_uint8_t read_pin_0, cycle;
  EXTI_ClearITPendingBit(EXTI_Line0);
  read_pin_0 = GPIO_ReadInputDataBit(GPIOA, B_MAIN);

  while(read_pin_0 == 1)
    {
      read_pin_0 = GPIO_ReadInputDataBit(GPIOA, B_MAIN);
      
      if(cycle == 50)
	{
	  schedule_callback_request_temperature();
	  
	  break;
	}
      cycle++;
    }
  cycle = 0;
}

//-----------------------------------------------------------------------

void set_request_for_send_temperature(zb_callback_t func)
{
  request_temperature_callback = func;
}

void schedule_callback_request_temperature(void)
{
  ZB_SCHEDULE_ALARM(request_temperature_callback, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}
