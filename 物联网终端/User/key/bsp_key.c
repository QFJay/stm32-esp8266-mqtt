#include "key/bsp_key.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState pressed_level;
} KeyConfig;

static const KeyConfig key_configs[KEY_COUNT] =
{
	{GPIOA, GPIO_PIN_6, GPIO_PIN_SET},	// KEY_1
	{GPIOA, GPIO_PIN_7, GPIO_PIN_SET},	// KEY_2
	{GPIOB, GPIO_PIN_0, GPIO_PIN_SET},	// KEY_3
};

void Key_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	for (uint8_t i = 0; i < KEY_COUNT; i++)
	{
		GPIO_InitStruct.Pin = key_configs[i].pin;
		GPIO_InitStruct.Pull = (key_configs[i].pressed_level == GPIO_PIN_SET) ? GPIO_PULLDOWN : GPIO_PULLUP;
		HAL_GPIO_Init(key_configs[i].port, &GPIO_InitStruct);
	}
}

static bool Key_IsKeyPressed(KeyId id)
{
	if (id >= KEY_COUNT)
	{
		return false;
	}

	return HAL_GPIO_ReadPin(key_configs[id].port, key_configs[id].pin) == key_configs[id].pressed_level;
}

bool Key_IsKeyClicked(KeyId id)
{
	static uint8_t count[KEY_COUNT] = {0};
	static bool pressed[KEY_COUNT] = {false};

	if (Key_IsKeyPressed(id) && !pressed[id])
	{
		count[id]++;
		if (count[id] >= KEY_DEBOUNCE_COUNT && Key_IsKeyPressed(id))
		{
			pressed[id] = true;
			return true;
		}
	}

	if (!Key_IsKeyPressed(id))
	{
		pressed[id] = false;
		count[id] = 0;
	}

	return false;
}
