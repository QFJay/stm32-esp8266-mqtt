#ifndef __BSP_FONTS_H
#define __BSP_FONTS_H

#include "main.h"

#define OLED_CHN_CHAR_WIDTH	3

typedef struct
{
	char index[OLED_CHN_CHAR_WIDTH + 1];
	uint8_t data[32];
} ChineseCell_t;

extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];

extern const ChineseCell_t OLED_CF16x16[];

#endif
