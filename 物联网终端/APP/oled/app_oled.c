#include "oled/bsp_i2c_oled.h"
#include "types/app_screen_type.h"
#include "types/app_sensor_data.h"
#include "cmsis_os2.h"

extern ScreenType global_screen_type;
extern DisplayData global_display_data;

static void OLED_DisplayHumidity(void)
{
	OLED_ShowString(0, 0, "[P1]Humidity", OLED_8X16);

	if (global_display_data.dht11_check_result == DHT11_DATA_OK)
	{
		OLED_Printf(0, 16, OLED_8X16, "Now: %+07.3f %%RH", global_display_data.humi_value);
	}
	else
	{
		OLED_ShowString(0, 16, "Now: DATA ERROR", OLED_8X16);
	}

	OLED_Printf(0, 32, OLED_8X16, "Max: %+07.3f %%RH", global_display_data.humi_max);
	OLED_Printf(0, 48, OLED_8X16, "Min: %+07.3f %%RH", global_display_data.humi_min);
}

static void OLED_DisplayTemperature(void)
{
	OLED_ShowString(0, 0, "[P2]Temperature", OLED_8X16);

	if (global_display_data.dht11_check_result == DHT11_DATA_OK)
	{
		OLED_Printf(0, 16, OLED_8X16, "Now: %+07.3f C", global_display_data.temp_value);
	}
	else
	{
		OLED_ShowString(0, 16, "Now: DATA ERROR", OLED_8X16);
	}

	OLED_Printf(0, 32, OLED_8X16, "Max: %+07.3f C", global_display_data.temp_max);
	OLED_Printf(0, 48, OLED_8X16, "Min: %+07.3f C", global_display_data.temp_min);
}

static void OLED_DisplayLight(void)
{
	OLED_ShowString(0, 0, "[P3]Light", OLED_8X16);
	OLED_Printf(0, 16, OLED_8X16, "Now: %06.2f %%", global_display_data.light_percentage_value);
	OLED_Printf(0, 32, OLED_8X16, "Max: %06.2f %%", global_display_data.light_percentage_max);
	OLED_Printf(0, 48, OLED_8X16, "Min: %06.2f %%", global_display_data.light_percentage_min);
}

typedef void (*OLEDScreenHandler)(void);

static const OLEDScreenHandler screen_handlers[SCREEN_TYPE_COUNT] =
{
	OLED_DisplayHumidity,
	OLED_DisplayTemperature,
	OLED_DisplayLight
};

void StartOLEDTask(void *argument)
{
	while (1)
	{
		OLED_Clear();
		screen_handlers[global_screen_type]();
		OLED_Update();
		osDelay(100);
	}
}
