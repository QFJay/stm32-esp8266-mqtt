#include "dht11/bsp_dht11.h"
#include "light/bsp_light.h"
#include "types/app_sensor_data.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"

extern osMessageQueueId_t UpdateDataQueueHandle;
extern osMessageQueueId_t ESP8266PackQueueHandle;

void StartSensorTask(void *argument)
{
	DHT11_RawData dht11_raw_data = {0};

	while (1)
	{
		SensorData *update_data = pvPortMalloc(sizeof(SensorData));
		if (update_data == NULL)
		{
			Error_Handler();
		}

		SensorData *ESP8266_data = pvPortMalloc(sizeof(SensorData));
		if (ESP8266_data == NULL)
		{
			Error_Handler();
		}

		/* Get DHT11 data */
		if (DHT11_ReadRawData(&dht11_raw_data) == HAL_OK)
		{
			update_data->dht11_check_result = DHT11_DATA_OK;

			/* Parse humidity */
			if (dht11_raw_data.humi_deci & 0x80)	// negative humidity
			{
				update_data->dht11_check_result = DHT11_DATA_ERROR;
			}
			else
			{
				update_data->humi_value = (float)dht11_raw_data.humi_int + (float)dht11_raw_data.humi_deci / 10.0f;
			}

			/* Parse temperature */
			if (dht11_raw_data.temp_deci & 0x80)	// negative temperature
			{
				uint8_t deci_value = dht11_raw_data.temp_deci & 0x7F;

				if (deci_value >= 100)
				{
					update_data->temp_value = ((float)dht11_raw_data.temp_int + (float)deci_value / 1000.0f) * (-1.0f);
				}
				else if (deci_value >= 10)
				{
					update_data->temp_value = ((float)dht11_raw_data.temp_int + (float)deci_value / 100.0f) * (-1.0f);
				}
				else
				{
					update_data->temp_value = ((float)dht11_raw_data.temp_int + (float)deci_value / 10.0f) * (-1.0f);
				} 
			}
			else
			{
				update_data->temp_value = (float)dht11_raw_data.temp_int + (float)dht11_raw_data.temp_deci / 10.0f;
			}
		}
		else
		{
			update_data->dht11_check_result = DHT11_DATA_ERROR;
		}

		/* Get light data */
		update_data->light_percentage_value = Light_ReadPercentageValue();

		*ESP8266_data = *update_data;
		osMessageQueuePut(UpdateDataQueueHandle, &update_data, 0, osWaitForever);
		osMessageQueuePut(ESP8266PackQueueHandle, &ESP8266_data, 0, osWaitForever);

		/* DHT11 updates data every 2 seconds */
		osDelay(2000);
	}
}
