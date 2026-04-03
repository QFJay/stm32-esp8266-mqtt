#ifndef __BSP_DWT_H
#define __BSP_DWT_H

#include "main.h"

void DWT_Init(void);
uint32_t DWT_GetTick(void);
uint32_t DWT_TickToMicrosecond(uint32_t tick, uint32_t frequency);
void DWT_DelayUs(uint32_t time);
void DWT_DelayMs(uint32_t time);
void DWT_DelayS(uint32_t time);

#endif
