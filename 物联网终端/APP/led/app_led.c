#include "led/bsp_led.h"
#include "types/app_sensor_data.h"
#include "cmsis_os2.h"

#define THRESHOLD_COUNT							3

#define HUMIDITY_THRESHOLD					80.0f
#define TEMPERATURE_THRESHOLD				35.0f
#define LIGHT_PERCENTAGE_THRESHOLD	30.0f

extern DisplayData global_display_data;

static void LED_HumidityAlarm(void)
{
	if (global_display_data.humi_value >= HUMIDITY_THRESHOLD)
	{
		LED_On(LED_RED);
	}
	else
	{
		LED_Off(LED_RED);
	}
}

static void LED_TemperatureAlarm(void)
{
	if (global_display_data.temp_value >= TEMPERATURE_THRESHOLD)
	{
		LED_On(LED_GREEN);
	}
	else
	{
		LED_Off(LED_GREEN);
	}
}

static void LED_LightAlarm(void)
{
	if (global_display_data.light_percentage_value <= LIGHT_PERCENTAGE_THRESHOLD)
	{
		LED_On(LED_BLUE);
	}
	else
	{
		LED_Off(LED_BLUE);
	}
}

typedef void (*LEDAlarmHandler)(void);

static const LEDAlarmHandler alarm_handlers[THRESHOLD_COUNT] =
{
	LED_HumidityAlarm,
	LED_TemperatureAlarm,
	LED_LightAlarm
};

void StartLEDTask(void *argument)
{
	while (1)
	{
		for (uint8_t i = 0; i < THRESHOLD_COUNT; i++)
		{
			alarm_handlers[i]();
		}

		osDelay(100);
	}
}
