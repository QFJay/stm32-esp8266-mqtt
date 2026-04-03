#ifndef __BSP_I2C_OLED_H
#define __BSP_I2C_OLED_H

#include "main.h"

typedef enum
{
	OLED_6X8 = 6,
	OLED_8X16 = 8
} OLEDFontSize;

typedef enum
{
	OLED_UNFILLED = 0,
	OLED_FILLED = 1
} OLEDIsFilled;

void OLED_Init(void);

void OLED_Update(void);

void OLED_Clear(void);
void OLED_ClearArea(int16_t x, int16_t y, uint8_t width, uint8_t height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t x, int16_t y, uint8_t width, uint8_t height);

void OLED_ShowImage(int16_t x, int16_t y, uint8_t width, uint8_t height, const uint8_t *image);
void OLED_ShowChar(int16_t x, int16_t y, char character, OLEDFontSize font_size);
void OLED_ShowString(int16_t x, int16_t y, char *string, OLEDFontSize font_size);
void OLED_ShowNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size);
void OLED_ShowSignedNum(int16_t x, int16_t y, int64_t number, uint8_t length, OLEDFontSize font_size);
void OLED_ShowHexNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size);
void OLED_ShowBinNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size);
void OLED_ShowFloatNum(int16_t x, int16_t y, double number, uint8_t int_length, uint8_t fra_length, OLEDFontSize font_size);
void OLED_ShowChinese(int16_t x, int16_t y, char *Chinese);
void OLED_Printf(int16_t x, int16_t y, OLEDFontSize font_size, char *format, ...);

void OLED_DrawPoint(int16_t x, int16_t y);
uint8_t OLED_GetPoint(int16_t x, int16_t y);
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void OLED_DrawRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height, OLEDIsFilled is_filled);
void OLED_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, OLEDIsFilled is_filled);
void OLED_DrawCircle(int16_t x, int16_t y, uint8_t radius, OLEDIsFilled is_filled);
void OLED_DrawEllipse(int16_t x, int16_t y, uint8_t a, uint8_t b, OLEDIsFilled is_filled);
void OLED_DrawArc(int16_t x, int16_t y, uint8_t radius, int16_t start_angle, int16_t end_angle, OLEDIsFilled is_filled);

#endif
