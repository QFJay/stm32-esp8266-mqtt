#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

void ESP8266_RX_DMA_Init(void);
void Debug_UART_Init(void);
void ESP8266_UART_Init(void);

#endif
