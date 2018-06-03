#include "LIB_INC/zdo_header_for_lcd.h"

static zb_uint8_t backlightState = 1;

void init_led(void)
{
  GPIO_InitTypeDef  led_init_struct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  led_init_struct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  led_init_struct.GPIO_Mode = GPIO_Mode_OUT;
  led_init_struct.GPIO_OType = GPIO_OType_PP;
  led_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  led_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &led_init_struct);
}

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




