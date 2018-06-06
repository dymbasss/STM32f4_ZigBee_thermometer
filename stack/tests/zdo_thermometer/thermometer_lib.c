#include "LIB_INC/zdo_header_for_thermometer.h"

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

zb_uint8_t value_temperature(void)
{
  // ADC Conversion to read temperature sensor
  // Temperature (in °C) = ((Vsense – V25) / Avg_Slope) + 25
  // Vsense = Voltage Reading From Temperature Sensor
  // V25 = Voltage at 25°C, for STM32F407 = 0.76V
  // Avg_Slope = 2.5mV/°C
  
  float value_temperature;
  zb_uint16_t V_sense = read_ADC1();
      
  value_temperature = V_sense;
  value_temperature *= 3; // The voltage with respect to which the comparison is made (my value - 3V, measured by a voltage sensor)
  value_temperature /= 4095; //Reading in V 
  value_temperature -= (float)0.760; // Subtract the reference voltage at 25°C
  value_temperature /= (float)0.0025; // Divide by slope 2.5mV
  value_temperature += (float)25.0; // Add the 25°C
  value_temperature -= (float)11.0; // Calibration_Value for measuring absolute temperature
  // out  value_temperature +- 1.5°C

  return value_temperature;
}


