#include "dht11/bsp_dht11.h"
#include "dwt/bsp_dwt.h"

#define DHT11_DATA_PIN				GPIO_PIN_12
#define DHT11_DATA_GPIO_PORT	GPIOB

void DHT11_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = DHT11_DATA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_DATA_GPIO_PORT, &GPIO_InitStruct);

  HAL_GPIO_WritePin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN, GPIO_PIN_SET);
}

static void DHT11_SetGPIOMode(uint32_t mode, uint32_t pull)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DHT11_DATA_PIN;
  GPIO_InitStruct.Mode = mode;
  GPIO_InitStruct.Pull = pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_DATA_GPIO_PORT, &GPIO_InitStruct);
}

static uint8_t DHT11_ReadByte(void)
{
	uint8_t byte = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_RESET);

		DWT_DelayUs(40);

		if (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_SET)
		{
			byte |= (0x80 >> i);

			while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_SET);
		}
	}

	return byte;
}

HAL_StatusTypeDef DHT11_ReadRawData(DHT11_RawData *data)
{
	uint8_t retry, sum;

	DHT11_SetGPIOMode(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
	HAL_GPIO_WritePin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN, GPIO_PIN_RESET);
	DWT_DelayMs(20);
	HAL_GPIO_WritePin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN, GPIO_PIN_SET);
	DWT_DelayUs(30);

	DHT11_SetGPIOMode(GPIO_MODE_INPUT, GPIO_PULLUP);

	retry = 0;
	while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_SET)
	{
		retry++;
		if (retry > 100)
		{
			return HAL_ERROR;
		}
		DWT_DelayUs(1);
	}

	retry = 0;
	while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_RESET)
	{
		retry++;
		if (retry > 100)
		{
			return HAL_ERROR;
		}
		DWT_DelayUs(1);
	}

	retry = 0;
	while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_PORT, DHT11_DATA_PIN) == GPIO_PIN_SET)
	{
		retry++;
		if (retry > 100)
		{
			return HAL_ERROR;
		}
		DWT_DelayUs(1);
	}

	data->humi_int = DHT11_ReadByte();
	data->humi_deci = DHT11_ReadByte();
	data->temp_int = DHT11_ReadByte();
	data->temp_deci = DHT11_ReadByte();
	data->check_sum = DHT11_ReadByte();

	sum = data->humi_int + data->humi_deci + data->temp_int + data->temp_deci;

	return (sum == data->check_sum) ? HAL_OK : HAL_ERROR;
}
