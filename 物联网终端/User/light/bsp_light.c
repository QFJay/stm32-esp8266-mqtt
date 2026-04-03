#include "light/bsp_light.h"
#include "adc/bsp_adc.h"

uint16_t Light_adc_value;
volatile bool Light_conversion_complete = false;

void Light_Init(void)
{
	Light_DMA_Init();
	Light_ADC1_Init();
	HAL_ADCEx_Calibration_Start(&hadc1);
}

float Light_ReadPercentageValue(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&Light_adc_value, 1);
	while (Light_conversion_complete == false);
	Light_conversion_complete = false;
	return (float)(4095 - Light_adc_value) / 4095.0f * 100.0f;
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1)
	{
		Light_conversion_complete = true;
	}
}
