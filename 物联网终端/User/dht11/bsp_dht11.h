#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "main.h"

typedef struct
{
	uint8_t humi_int;
	uint8_t humi_deci;
	uint8_t temp_int;
	uint8_t temp_deci;
	uint8_t check_sum;
} DHT11_RawData;

void DHT11_Init(void);
HAL_StatusTypeDef DHT11_ReadRawData(DHT11_RawData *data);

#endif
