#include "key/bsp_key.h"
#include "types/app_screen_type.h"
#include "cmsis_os2.h"

ScreenType global_screen_type = SCREEN_TYPE_HUMIDITY;

void StartKeyTask(void *argument)
{
	while (1)
	{
		if (Key_IsKeyClicked(KEY_1))
		{
			global_screen_type = (ScreenType)((global_screen_type + 1) % SCREEN_TYPE_COUNT);
		}

		osDelay(KEY_CHECK_INTERVAL);
	}
}
