#include "LIB_INC/zdo_header_for_lcd.h"

void schedule_callback(zb_uint8_t param);

static zb_uint8_t backlightState = 1;
static volatile zb_bool_t button_state;

zb_callback_t request_temperature_callback;

//------------------------------------------------------------------------

void init_pin(void)
{
  GPIO_InitTypeDef  pin_init_struct;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* Init leds */
  pin_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  pin_init_struct.GPIO_Mode = GPIO_Mode_AF;
  pin_init_struct.GPIO_OType = GPIO_OType_OD;
  pin_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
  pin_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &pin_init_struct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
}

//-----------------------------------------------------------------------

void init_i2c(void)
{
  I2C_InitTypeDef i2c_init_struct;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  i2c_init_struct.I2C_ClockSpeed = 50000;
  i2c_init_struct.I2C_Mode = I2C_Mode_I2C;
  i2c_init_struct.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_init_struct.I2C_OwnAddress1 = 0x00;
  i2c_init_struct.I2C_Ack = I2C_Ack_Enable;
  i2c_init_struct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &i2c_init_struct);
  I2C_Cmd(I2C1, ENABLE);
}

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
	  schedule_callback(0);
	  
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

void schedule_callback(zb_uint8_t param)
{
  ZB_SCHEDULE_ALARM(request_temperature_callback, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}

void delay_ms(zb_uint16_t ms)
{
  volatile zb_uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
   
  nCount = (RCC_Clocks.HCLK_Frequency/168000) / ms;
  
  while(nCount != 0)
    {
      nCount--;
    }
}

void lcd_init(void)
{
  lcd_command(0x33);
  lcd_pause;
  
  lcd_command(0x32); // set mode : 4 lines
  lcd_command(0x28); // 2 strings and 5*8 pixels
  lcd_command(0x08); // on lcd
  lcd_command(0x01); // reset lcd
  lcd_pause;
  
  lcd_command(0x06); // if writing, | -> on left
  lcd_command(0x0C); // on lcd
}

void lcd_goto(zb_uint8_t row, zb_uint8_t col)
{
#ifdef LCD_1602
  switch (row)
  {
  case 1:
    lcd_command(0x80 + col);
    break;
  case 2:
    lcd_command(0x80 + col + 0x40);
    break;
  }
#endif
}

void lcd_print(zb_uint8_t *str)
{
  while (*str)
    {
      lcd_data(*str++);
    }
}

void lcd_send(zb_uint8_t data)
{
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C1, ENABLE);

  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C1, ((0x20+LCD_ADDR) << 1), I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C1, data);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTOP(I2C1, ENABLE);
}

void lcd_command(zb_uint8_t com)
{
  zb_uint8_t data = 0;

  data |= (backlightState & 0x01) << BL;

  data |= (((com & 0x10) >> 4) << DB4);
  data |= (((com & 0x20) >> 5) << DB5);
  data |= (((com & 0x40) >> 6) << DB6);
  data |= (((com & 0x80) >> 7) << DB7);
  lcd_send(data);

  data |= (1 << EN);
  lcd_send(data);
  lcd_pause;

  data &= ~(1 << EN);
  lcd_send(data);
  lcd_pause;

  data = 0;

  data |= (backlightState & 0x01) << BL;

  data |= (((com & 0x01) >> 0) << DB4);
  data |= (((com & 0x02) >> 1) << DB5);
  data |= (((com & 0x04) >> 2) << DB6);
  data |= (((com & 0x08) >> 3) << DB7);
  lcd_send(data);

  data |= (1 << EN);
  lcd_send(data);
  lcd_pause;

  data &= ~(1 << EN);
  lcd_send(data);
  lcd_pause;
}

void lcd_backlight(zb_uint8_t state)
{
  backlightState = (state & 0x01) << BL;
  lcd_send(backlightState);
}

void lcd_data(zb_uint8_t com)
{
  zb_uint8_t data = 0;

  data |= (1 << EN);
  data |= (1 << RS);
  data |= (backlightState & 0x01) << BL;

  data |= (((com & 0x10) >> 4) << DB4);
  data |= (((com & 0x20) >> 5) << DB5);
  data |= (((com & 0x40) >> 6) << DB6);
  data |= (((com & 0x80) >> 7) << DB7);
  lcd_send(data);
  lcd_pause;

  data &= ~(1 << EN);
  lcd_send(data);
  lcd_pause;

  data = 0;

  data |= (1 << EN);
  data |= (1 << RS);
  data |= (backlightState & 0x01) << BL;

  data |= (((com & 0x01) >> 0) << DB4);
  data |= (((com & 0x02) >> 1) << DB5);
  data |= (((com & 0x04) >> 2) << DB6);
  data |= (((com & 0x08) >> 3) << DB7);
  lcd_send(data);
  lcd_pause;

  data &= ~(1 << EN);
  lcd_send(data);
  lcd_pause;
}




