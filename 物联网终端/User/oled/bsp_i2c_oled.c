#include "oled/bsp_i2c_oled.h"
#include "i2c/bsp_i2c.h"
#include "fonts/bsp_fonts.h"
#include "usart/bsp_usart.h"

#define OLED_WR_CMD			0x00
#define OLED_WR_DATA		0x40

#define OLED_SLAVE_ADDR	0x3C

#define I2C_TIMEOUT			100

uint8_t OLED_DisplayBuf[8][128];

static ErrorStatus OLED_CheckDevice(void)
{
	uint8_t dummy = 0;

	if (HAL_I2C_Master_Transmit(&hi2c1, (OLED_SLAVE_ADDR << 1), &dummy, 0, I2C_TIMEOUT) == HAL_OK)
	{
//		printf("[OLED_CheckDevice] OLED has passed the test.\r\n");
		return SUCCESS;
	}

	return ERROR;
}

static ErrorStatus OLED_WriteCommand(uint8_t command)
{
	uint8_t buf[2] = {OLED_WR_CMD, command};

	if (HAL_I2C_Master_Transmit(&hi2c1, (OLED_SLAVE_ADDR << 1), buf, 2, I2C_TIMEOUT) == HAL_OK)
	{
		return SUCCESS;
	}

	printf("[OLED_WriteCommand] Write failed: command = 0x%02X\r\n", command);
	return ERROR;
}

static ErrorStatus OLED_WriteData(uint8_t *data, uint16_t count)
{
	uint8_t buf[256];

	if (count > sizeof(buf) - 1)
	{
		printf("[OLED_WriteData] Data overload: count = %d\r\n", count);
		return ERROR;
	}

	buf[0] = OLED_WR_DATA;
	memcpy(&buf[1], data, count);

	if (HAL_I2C_Master_Transmit(&hi2c1, (OLED_SLAVE_ADDR << 1), buf, (count + 1), I2C_TIMEOUT) == HAL_OK)
	{
		return SUCCESS;
	}

	printf("[OLED_WriteData] Write failed: count = %d\r\n", count);
	return ERROR;
}

void OLED_Init(void)
{
	HAL_Delay(100);

	while (OLED_CheckDevice() != SUCCESS);

	OLED_WriteCommand(0xAE);

	OLED_WriteCommand(0xD5);
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8);
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3);
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);

	OLED_WriteCommand(0xA1);

	OLED_WriteCommand(0xC8);

	OLED_WriteCommand(0xDA);
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81);
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);

	OLED_WriteCommand(0xA6);

	OLED_WriteCommand(0x8D);
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);

	OLED_Clear();
	OLED_Update();
}

static void OLED_SetCursor(uint8_t x, uint8_t page)
{
	OLED_WriteCommand(0x00 | (x & 0x0F));
	OLED_WriteCommand(0x10 | ((x & 0xF0) >> 4));
	OLED_WriteCommand(0xB0 | page);
}

void OLED_Update(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		OLED_SetCursor(0, i);
		OLED_WriteData(OLED_DisplayBuf[i], 128);
	}
}

void OLED_Clear(void)
{
	uint8_t i, j;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 128; j++)
		{
			OLED_DisplayBuf[i][j] = 0x00;
		}
	}
}

void OLED_ClearArea(int16_t x, int16_t y, uint8_t width, uint8_t height)
{
	int16_t i, j;

	for (i = y; i < y + height; i++)
	{
		for (j = x; j < x + width; j++)
		{
			if (i >= 0 && i <= 63 && j >= 0 && j <= 127)
			{
				OLED_DisplayBuf[i / 8][j] &= ~(0x01 << (i % 8));
			}
		}
	}
}

void OLED_Reverse(void)
{
	uint8_t i, j;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 128; j++)
		{
			OLED_DisplayBuf[i][j] ^= 0xFF;
		}
	}
}

void OLED_ReverseArea(int16_t x, int16_t y, uint8_t width, uint8_t height)
{
	int16_t i, j;

	for (i = y; i < y + height; i++)
	{
		for (j = x; j < x + width; j++)
		{
			if (i >= 0 && i <= 63 && j >= 0 && j <= 127)
			{
				OLED_DisplayBuf[i / 8][j] ^= (0x01 << (i % 8));
			}
		}
	}
}

void OLED_ShowImage(int16_t x, int16_t y, uint8_t width, uint8_t height, const uint8_t *image)
{
	int16_t i, j;
	int16_t page = y / 8;
	int16_t shift = y % 8;

	if (y < 0)
	{
		page -= 1;
		shift += 8;
	}

	OLED_ClearArea(x, y, width, height);

	for (i = 0; i < (height - 1) / 8 + 1; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (x + j >= 0 && x + j <= 127)
			{
				if (page + i >= 0 && page + i <= 7)
				{
					OLED_DisplayBuf[page + i][x + j] |= (image[i * width + j] << shift);
				}

				if (page + i + 1 >= 0 && page + i + 1 <= 7)
				{
					OLED_DisplayBuf[page + i + 1][x + j] |= (image[i * width + j] >> (8 - shift));
				}
			}
		}
	}
}

void OLED_ShowChar(int16_t x, int16_t y, char character, OLEDFontSize font_size)
{
	if (font_size == OLED_6X8)
	{
		OLED_ShowImage(x, y, 6, 8, OLED_F6x8[character - ' ']);
	}
	else if (font_size == OLED_8X16)
	{
		OLED_ShowImage(x, y, 8, 16, OLED_F8x16[character - ' ']);
	}
}

void OLED_ShowString(int16_t x, int16_t y, char *string, OLEDFontSize font_size)
{
	uint8_t i;

	for (i = 0; string[i] != '\0'; i++)
	{
		OLED_ShowChar(x + i * font_size, y, string[i], font_size);
	}
}

static uint64_t OLED_Pow(uint64_t x, uint64_t y)
{
	uint64_t result = 1;

	while (y--)
	{
		result *= x;
	}
	return result;
}

void OLED_ShowNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size)
{
	uint8_t i;
	char character;

	for (i = 0; i < length; i++)
	{
		character = number / OLED_Pow(10, length - i - 1) % 10 + '0';
		OLED_ShowChar(x + i * font_size, y, character, font_size);
	}
}

void OLED_ShowSignedNum(int16_t x, int16_t y, int64_t number, uint8_t length, OLEDFontSize font_size)
{
	uint8_t i;
	uint64_t unsigned_number;
	char character;

	if (number >= 0)
	{
		OLED_ShowChar(x, y, '+', font_size);
		unsigned_number = number;
	}
	else
	{
		OLED_ShowChar(x, y, '-', font_size);
		unsigned_number = -number;
	}

	for (i = 0; i < length; i++)
	{
		character = unsigned_number / OLED_Pow(10, length - i - 1) % 10 + '0';
		OLED_ShowChar(x + (i + 1) * font_size, y, character, font_size);
	}
}

void OLED_ShowHexNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size)
{
	uint8_t i, single_number;

	for (i = 0; i < length; i++)
	{
		single_number = number / OLED_Pow(16, length - i - 1) % 16;

		if (single_number < 10)
		{
			OLED_ShowChar(x + i * font_size, y, single_number + '0', font_size);
		}
		else
		{
			OLED_ShowChar(x + i * font_size, y, single_number - 10 + 'A', font_size);
		}
	}
}

void OLED_ShowBinNum(int16_t x, int16_t y, uint64_t number, uint8_t length, OLEDFontSize font_size)
{
	uint8_t i;
	char character;

	for (i = 0; i < length; i++)
	{
		character = number / OLED_Pow(2, length - i - 1) % 2 + '0';
		OLED_ShowChar(x + i * font_size, y, character, font_size);
	}
}

void OLED_ShowFloatNum(int16_t x, int16_t y, double number, uint8_t int_length, uint8_t fra_length, OLEDFontSize font_size)
{
	uint64_t int_number, fra_number, pow_number;

	if (number >= 0)
	{
		OLED_ShowChar(x, y, '+', font_size);
	}
	else
	{
		OLED_ShowChar(x, y, '-', font_size);
		number = -number;
	}

	int_number = number;
	number -= int_number;
	pow_number = OLED_Pow(10, fra_length);
	fra_number = round(number * pow_number);
	if (fra_number / pow_number == 1)
	{
		int_number++;
	}

	OLED_ShowNum(x + font_size, y, int_number, int_length, font_size);
	OLED_ShowChar(x + (int_length + 1) * font_size, y, '.', font_size);
	OLED_ShowNum(x + (int_length + 2) * font_size, y, fra_number, fra_length, font_size);
}

void OLED_ShowChinese(int16_t x, int16_t y, char *Chinese)
{
	uint8_t i, j;
	uint8_t pChinese = 0;
	uint8_t Chinese_count = 0;
	char single_Chinese[OLED_CHN_CHAR_WIDTH + 1] = {0};

	for (i = 0; Chinese[i] != '\0'; i++)
	{
		single_Chinese[pChinese] = Chinese[i];
		pChinese++;

		if (pChinese >= OLED_CHN_CHAR_WIDTH)
		{
			for (j = 0; strcmp(OLED_CF16x16[j].index, "") != 0; j++)
			{
				if (strcmp(OLED_CF16x16[j].index, single_Chinese) == 0)
				{
					break;
				}
			}
			OLED_ShowImage(x + Chinese_count * 16, y, 16, 16, OLED_CF16x16[j].data);
			Chinese_count++;
			pChinese = 0;
		}
	}
}

void OLED_Printf(int16_t x, int16_t y, OLEDFontSize font_size, char *format, ...)
{
	char string[64];
	va_list arg;
	va_start(arg, format);
	vsprintf(string, format, arg);
	va_end(arg);

	OLED_ShowString(x, y, string, font_size);
}

void OLED_DrawPoint(int16_t x, int16_t y)
{
	if (x >= 0 && x <= 127 && y >= 0 && y <= 63)
	{
		OLED_DisplayBuf[y / 8][x] |= (0x01 << (y % 8));
	}
}

uint8_t OLED_GetPoint(int16_t x, int16_t y)
{
	if (x >= 0 && x <= 127 && y >= 0 && y <= 63)
	{
		if (OLED_DisplayBuf[y / 8][x] & (0x01 << (y % 8)))
		{
			return 1;
		}
	}

	return 0;
}

static void OLED_Swap(int16_t *a, int16_t *b)
{
	int16_t temp = *a;
	*a = *b;
	*b = temp;
}

void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int16_t x, y;
	float k;

	if (y0 == y1)
	{
		if (x0 > x1)
		{
			OLED_Swap(&x0, &x1);
		}

		for (x = x0; x <= x1; x++)
		{
			OLED_DrawPoint(x, y0);
		}
	}
	else if (x0 == x1)
	{
		if (y0 > y1)
		{
			OLED_Swap(&y0, &y1);
		}

		for (y = y0; y <= y1; y++)
		{
			OLED_DrawPoint(x0, y);
		}
	}
	else
	{
		k = (float)(y1 - y0) / (x1 - x0);

		if (fabs(k) < 1)
		{
			if (x0 > x1)
			{
				OLED_Swap(&x0, &x1);
				OLED_Swap(&y0, &y1);
			}

			for (x = x0; x <= x1; x++)
			{
				OLED_DrawPoint(x, round(y0 + (x - x0) * k));
			}
		}
		else
		{
			if (y0 > y1)
			{
				OLED_Swap(&x0, &x1);
				OLED_Swap(&y0, &y1);
			}

			for (y = y0; y <= y1; y++)
			{
				OLED_DrawPoint(round(x0 + (y - y0) / k), y);
			}
		}
	}
}

void OLED_DrawRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height, OLEDIsFilled is_filled)
{
	uint8_t i;

	if (width == 0 || height == 0) {return;}

	if (is_filled == OLED_UNFILLED)
	{
		OLED_DrawLine(x, y, x + width - 1, y);
		OLED_DrawLine(x, y, x, y + height - 1);
		OLED_DrawLine(x, y + height - 1, x + width - 1, y + height - 1);
		OLED_DrawLine(x + width - 1, y, x + width - 1, y + height - 1);
	}
	else
	{
		for (i = 0; i < height; i++)
		{
			OLED_DrawLine(x, y + i, x + width - 1, y + i);
		}
	}
}

static int16_t OLED_GetMin(int16_t a, int16_t b)
{
    return (a < b) ? a : b;
}

static int16_t OLED_GetMax(int16_t a, int16_t b)
{
	return (a > b) ? a : b;
}

static uint8_t OLED_PointInPolygon(uint8_t vert_number, int16_t *vert_x, int16_t *vert_y, int16_t test_x, int16_t test_y)
{
	int16_t i, j, c = 0;

	for (i = 0, j = vert_number - 1; i < vert_number; j = i++)
	{
		if (((vert_y[i] > test_y) != (vert_y[j] > test_y)) && (test_x < (vert_x[j] - vert_x[i]) * (test_y - vert_y[i]) / (vert_y[j] - vert_y[i]) + vert_x[i]))
		{
			c = !c;
		}
	}

	return c;
}

void OLED_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, OLEDIsFilled is_filled)
{
	int16_t i, j, min_x, min_y, max_x, max_y;
	int16_t vx[] = {x0, x1, x2};
	int16_t vy[] = {y0, y1, y2};

	if (is_filled == OLED_UNFILLED)
	{
		OLED_DrawLine(x0, y0, x1, y1);
		OLED_DrawLine(x0, y0, x2, y2);
		OLED_DrawLine(x1, y1, x2, y2);
	}
	else
	{
		min_x = OLED_GetMin(OLED_GetMin(x0, x1), x2);
		min_y = OLED_GetMin(OLED_GetMin(y0, y1), y2);
		max_x = OLED_GetMax(OLED_GetMax(x0, x1), x2);
		max_y = OLED_GetMax(OLED_GetMax(y0, y1), y2);

		for (i = min_x; i <= max_x; i++)
		{
			for (j = min_y; j <= max_y; j++)
			{
				if (OLED_PointInPolygon(3, vx, vy, i, j))
				{
					OLED_DrawPoint(i, j);
				}
			}
		}
	}
}

void OLED_DrawCircle(int16_t x, int16_t y, uint8_t radius, OLEDIsFilled is_filled)
{
	uint8_t i;
	int16_t x0, x1, y0, y1;
	float angle, dx, dy;
	const float DEGREES_TO_RADIANS = 2.0f * 3.14159265f / 360.0f;

	if (radius == 0)
	{
		OLED_DrawPoint(x, y);
		return;
	}

	for (i = 0; i <= 90; i++)
	{
		angle = i * DEGREES_TO_RADIANS;
		dx = cosf(angle) * radius;
		dy = sinf(angle) * radius;
		x0 = round(x + dx);
		x1 = round(x - dx);
		y0 = round(y + dy);
		y1 = round(y - dy);

		if (is_filled == OLED_UNFILLED)
		{
			OLED_DrawPoint(x0, y0);
			OLED_DrawPoint(x0, y1);
			OLED_DrawPoint(x1, y0);
			OLED_DrawPoint(x1, y1);
		}
		else
		{
			OLED_DrawLine(x0, y0, x1, y0);
			OLED_DrawLine(x0, y1, x1, y1);
		}
	}
}

void OLED_DrawEllipse(int16_t x, int16_t y, uint8_t a, uint8_t b, OLEDIsFilled is_filled)
{
	uint8_t i;
	int16_t x0, x1, y0, y1;
	float angle, dx, dy;
	const float DEGREES_TO_RADIANS = 2.0f * 3.14159265f / 360.0f;

	if (a == 0 && b == 0)
	{
        OLED_DrawPoint(x, y);
        return;
    }
    else if (a == 0)
	{
        OLED_DrawLine(x, y - b, x, y + b);
        return;
    }
    else if (b == 0)
	{
        OLED_DrawLine(x - a, y, x + a, y);
        return;
    }

	for (i = 0; i <= 90; i++)
	{
		angle = i * DEGREES_TO_RADIANS;
		dx = cosf(angle) * a;
		dy = sinf(angle) * b;
		x0 = round(x + dx);
		x1 = round(x - dx);
		y0 = round(y + dy);
		y1 = round(y - dy);

		if (is_filled == OLED_UNFILLED)
		{
			OLED_DrawPoint(x0, y0);
			OLED_DrawPoint(x0, y1);
			OLED_DrawPoint(x1, y0);
			OLED_DrawPoint(x1, y1);
		}
		else
		{
			OLED_DrawLine(x0, y0, x1, y0);
			OLED_DrawLine(x0, y1, x1, y1);
		}
	}
}

void OLED_DrawArc(int16_t x, int16_t y, uint8_t radius, int16_t start_angle, int16_t end_angle, OLEDIsFilled is_filled)
{
	int16_t i, r;
	float angle, cos_result, sin_result;
	const float DEGREES_TO_RADIANS = 2.0f * 3.14159265f / 360.0f;

	if (start_angle < -180 || start_angle > 180 || end_angle < -180 || end_angle > 180) {return;}

	if (radius == 0)
	{
		OLED_DrawPoint(x, y);
		return;
	}

	if (end_angle < start_angle)
	{
		end_angle += 360;
	}

	for (i = start_angle; i <= end_angle; i++)
	{
		angle = i * DEGREES_TO_RADIANS;
		cos_result = cosf(angle);
		sin_result = sinf(angle);

		if (is_filled == OLED_UNFILLED)
		{
			OLED_DrawPoint(round(x + cos_result * radius), round(y + sin_result * radius));
		}
		else
		{
			for (r = 0; r <= radius; r++)
			{
				OLED_DrawPoint(round(x + cos_result * r), round(y + sin_result * r));
			}
		}
	}
}
