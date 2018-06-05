#include "LIB_INC/zdo_header_for_thermometer.h"

void calculation_of_temperature(void);
void on_off_timer(zb_uint8_t state);

static zb_uint8_t time;
static float value_temperature;

zb_callback_t temperature_callback;

//------------------------------------------------------------------------

void init_timer(void)
{
  TIM_TimeBaseInitTypeDef  t_init_struct;
  NVIC_InitTypeDef t_nvic_init_struct;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  t_init_struct.TIM_Period = 1000000 - 1;
  t_init_struct.TIM_Prescaler = 84 - 1;
  t_init_struct.TIM_ClockDivision = 0;
  t_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &t_init_struct);
  /*Interruption on up-dating*/
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  on_off_timer(1);

  t_nvic_init_struct.NVIC_IRQChannel = TIM2_IRQn;
  t_nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0;
  t_nvic_init_struct.NVIC_IRQChannelSubPriority = 1;
  t_nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&t_nvic_init_struct);  
}

void on_off_timer(zb_uint8_t state)
{
  switch(state)
    {
    case TIM_CMD_DISABLE:
      TIM_Cmd(TIM2, DISABLE);
      break;
    case TIM_CMD_ENABLE:
      TIM_Cmd(TIM2, ENABLE);
      break;
    }
}

void timer_run_time(zb_uint8_t value)
{
  if(value == 30) // 30 seconds
    {
      time = 0;
      schedule_callback(0);
    } 
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
      time++;
      timer_run_time(time);
    }
}

//------------------------------------------------------------------------

void init_adc(void) 
{
  ADC_InitTypeDef adc_init_struct;
  ADC_CommonInitTypeDef adc_common_init_struct;

  ADC_StructInit(&adc_init_struct);
  ADC_CommonStructInit(&adc_common_init_struct);
  ADC_DeInit(); // reset settings ADC

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  adc_common_init_struct.ADC_Mode = ADC_Mode_Independent;
  adc_common_init_struct.ADC_Prescaler = ADC_Prescaler_Div2;
  adc_common_init_struct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  adc_common_init_struct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&adc_common_init_struct);

  adc_init_struct.ADC_Resolution = ADC_Resolution_12b;
  adc_init_struct.ADC_ScanConvMode = DISABLE;
  adc_init_struct.ADC_ContinuousConvMode = DISABLE;
  adc_init_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  adc_init_struct.ADC_DataAlign = ADC_DataAlign_Right;
  adc_init_struct.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &adc_init_struct);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_3Cycles); // ADC1 Configuration, ADC_Channel_TempSensor is actual channel 16
  
  ADC_Cmd(ADC1, ENABLE); // Enable ADC conversion

  ADC_TempSensorVrefintCmd(ENABLE); // Enable internal temperature sensor
}

zb_uint16_t read_ADC1()
{
  ADC_SoftwareStartConv(ADC1); // start working

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // wait while the voltage is converted

  return ADC_GetConversionValue(ADC1); // return result
}

//------------------------------------------------------------------------

zb_uint8_t value_temperature_broadcast(void)
{ 
  return value_temperature;
}

void set_temperature_for_send(zb_callback_t func)
{
  temperature_callback = func;
}

void schedule_callback(zb_uint8_t param)
{ 
  calculation_of_temperature();
  ZB_SCHEDULE_ALARM(temperature_callback, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}

void calculation_of_temperature(void)
{
  // ADC Conversion to read temperature sensor
  // Temperature (in °C) = ((Vsense – V25) / Avg_Slope) + 25
  // Vsense = Voltage Reading From Temperature Sensor
  // V25 = Voltage at 25°C, for STM32F407 = 0.76V
  // Avg_Slope = 2.5mV/°C
      
  zb_uint16_t V_sense = read_ADC1();
      
  value_temperature = V_sense;
  value_temperature *= 3; // The voltage with respect to which the comparison is made (my value - 3V, measured by a voltage sensor)
  value_temperature /= 4095; //Reading in V 
  value_temperature -= (float)0.760; // Subtract the reference voltage at 25°C
  value_temperature /= (float)0.0025; // Divide by slope 2.5mV
  value_temperature += (float)25.0; // Add the 25°C
  value_temperature -= (float)11.0; // Calibration_Value for measuring absolute temperature
  // out  value_temperature +- 1.5°C
}


