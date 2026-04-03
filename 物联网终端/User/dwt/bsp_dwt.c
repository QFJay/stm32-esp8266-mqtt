#include "dwt/bsp_dwt.h"

#define DEMCR								*(uint32_t *)(0xE000EDFC)
#define DWT_CTRL						*(uint32_t *)(0xE0001000)
#define DWT_CYCCNT					*(uint32_t *)(0xE0001004)

#define DEMCR_TRCENA				(1 << 24)
#define DWT_CTRL_CYCCNTENA	(1 << 0)

void DWT_Init(void)
{
	/* Enable DWT device */
	DEMCR |= (uint32_t)DEMCR_TRCENA;

	/* Clear DWT CYCCNT register */
	DWT_CYCCNT = (uint32_t)0U;

	/* Enable Cortex-M DWT CYCCNT register */
	DWT_CTRL |= (uint32_t)DWT_CTRL_CYCCNTENA;
}

uint32_t DWT_GetTick(void)
{
	return (uint32_t)DWT_CYCCNT;
}

uint32_t DWT_TickToMicrosecond(uint32_t tick, uint32_t frequency)
{
	return (uint32_t)(1000000.0 / frequency * tick);
}

void DWT_DelayUs(uint32_t time)
{
	uint32_t tick_duration = time * (SystemCoreClock / 1000000);
	uint32_t tick_start = DWT_GetTick();
	while (DWT_GetTick() - tick_start < tick_duration);
}

void DWT_DelayMs(uint32_t time)
{
	for (uint32_t i = 0; i < time; i++)
	{
		DWT_DelayUs(1000);
	}
}

void DWT_DelayS(uint32_t time)
{
	for (uint32_t i = 0; i < time; i++)
	{
		DWT_DelayMs(1000);
	}
}
