#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "main.h"

#define KEY_CHECK_INTERVAL	10
#define KEY_DEBOUNCE_TIME		30
#define KEY_DEBOUNCE_COUNT	(KEY_DEBOUNCE_TIME / KEY_CHECK_INTERVAL)

typedef enum
{
	KEY_1 = 0,
	KEY_2,
	KEY_3,
	KEY_COUNT
} KeyId;

void Key_Init(void);
bool Key_IsKeyClicked(KeyId id);

#endif
