#include "types/app_sensor_data.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "main.h"

extern osMessageQueueId_t UpdateDataQueueHandle;

DisplayData global_display_data =
{
	.dht11_check_result = DHT11_DATA_ERROR,
	.humi_value = 0.0f,
	.humi_max = -FLT_MAX,
	.humi_min = FLT_MAX,
	.temp_value = 0.0f,
	.temp_max = -FLT_MAX,
	.temp_min = FLT_MAX,
	.light_percentage_value = 0.0f,
	.light_percentage_max = -FLT_MAX,
	.light_percentage_min = FLT_MAX
};

static void DHT11_UpdateMaxAndMin(void)
{
	if (global_display_data.humi_value > global_display_data.humi_max)
	{
		global_display_data.humi_max = global_display_data.humi_value;
	}

	if (global_display_data.humi_value < global_display_data.humi_min)
	{
		global_display_data.humi_min = global_display_data.humi_value;
	}

	if (global_display_data.temp_value > global_display_data.temp_max)
	{
		global_display_data.temp_max = global_display_data.temp_value;
	}

	if (global_display_data.temp_value < global_display_data.temp_min)
	{
		global_display_data.temp_min = global_display_data.temp_value;
	}
}

static void Light_UpdateMaxAndMin(void)
{
	if (global_display_data.light_percentage_value > global_display_data.light_percentage_max)
	{
		global_display_data.light_percentage_max = global_display_data.light_percentage_value;
	}

	if (global_display_data.light_percentage_value < global_display_data.light_percentage_min)
	{
		global_display_data.light_percentage_min = global_display_data.light_percentage_value;
	}
}

void StartUpdateDataTask(void *argument)
{
	while (1)
	{
		SensorData *sensor_data;

		osMessageQueueGet(UpdateDataQueueHandle, &sensor_data, 0, osWaitForever);
		global_display_data.dht11_check_result = sensor_data->dht11_check_result;

		if (global_display_data.dht11_check_result == DHT11_DATA_OK)
		{
			global_display_data.humi_value = sensor_data->humi_value;
			global_display_data.temp_value = sensor_data->temp_value;
			DHT11_UpdateMaxAndMin();
		}

		global_display_data.light_percentage_value = sensor_data->light_percentage_value;
		Light_UpdateMaxAndMin();

		vPortFree(sensor_data);
		sensor_data = NULL;
	}
}
