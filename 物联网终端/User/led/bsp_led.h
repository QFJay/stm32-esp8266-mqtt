#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "main.h"

typedef enum
{
	LED_RED = 0,
	LED_GREEN,
	LED_BLUE,
	LED_ONBOARD,
	LED_COUNT
} LEDId;

void LED_Init(void);
void LED_On(LEDId id);
void LED_Off(LEDId id);
void LED_Toggle(LEDId id);

#endif
