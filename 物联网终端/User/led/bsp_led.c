#include "led/bsp_led.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState on_level;
} LEDConfig;

static const LEDConfig LED_configs[LED_COUNT] =
{
	{GPIOA, GPIO_PIN_1, GPIO_PIN_RESET},	// LED_RED
	{GPIOA, GPIO_PIN_2, GPIO_PIN_RESET},	// LED_GREEN
	{GPIOA, GPIO_PIN_3, GPIO_PIN_RESET},	// LED_BLUE
	{GPIOC, GPIO_PIN_13, GPIO_PIN_RESET},	// LED_ONBOARD
};

void LED_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	for (uint8_t i = 0; i < LED_COUNT; i++)
	{
		GPIO_InitStruct.Pin = LED_configs[i].pin;
		HAL_GPIO_Init(LED_configs[i].port, &GPIO_InitStruct);
		LED_Off((LEDId)i);
	}
}

void LED_On(LEDId id)
{
	if (id >= LED_COUNT)
	{
		return;
	}

	HAL_GPIO_WritePin(LED_configs[id].port, LED_configs[id].pin, LED_configs[id].on_level);
}

void LED_Off(LEDId id)
{
	if (id >= LED_COUNT)
	{
		return;
	}

	HAL_GPIO_WritePin(LED_configs[id].port, LED_configs[id].pin, (GPIO_PinState)(!LED_configs[id].on_level));
}

void LED_Toggle(LEDId id)
{
	if (id >= LED_COUNT)
	{
		return;
	}

	HAL_GPIO_TogglePin(LED_configs[id].port, LED_configs[id].pin);
}
