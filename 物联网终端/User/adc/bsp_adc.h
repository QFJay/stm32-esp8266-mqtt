#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void Light_DMA_Init(void);
void Light_ADC1_Init(void);

#endif
