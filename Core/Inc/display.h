/*
 * display.h
 *
 *  Created on: Oct 3, 2020
 *      Author: Marcin
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"
#include "spi.h"

#define DISPLAY_SPI &hspi4

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 480
#define DISPLAY_BPP 3  //Bytes per pixel

#define MAX_BUFFER_SIZE 320

#if DISPLAY_BPP == 3
typedef struct
{
	union
	{
		struct
		{
			uint8_t b, g, r;
		};
		uint8_t arr[3];
	};
} Color_t;
#define IPFARGUMENT 0x66

#elif DISPLAY_BPP == 2
typedef struct
{
	union
	{
		struct
		{
			uint16_t b :5, g :6, r :5;
		};
		uint8_t arr[2];
	};
} Color_t;
#define IPFARGUMENT 0x55
#endif

Color_t makeColor(uint8_t red, uint8_t green, uint8_t blue);

void display_init();
void display_paint_red();
void display_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		Color_t color);
void display_fillScreen(Color_t color);


void display_startSendingImageFullScreen();
void display_startSendingImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void display_sendImagePart(uint8_t *buffer, uint32_t size);
void display_sendImagePartAsync(uint8_t *buffer, uint32_t size);
void display_setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);


void display_SPIStart();
void display_SPIStop();

#endif /* INC_DISPLAY_H_ */
