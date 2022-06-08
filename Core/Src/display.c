/*
 * display.c
 *
 *  Created on: Oct 3, 2020
 *      Author: Marcin
 */

#include "display.h"

#include "spi.h"

typedef struct
{
	uint8_t cmd;
	uint8_t argc;
	uint8_t args[];
} DISPLAY_cmd;

DISPLAY_cmd cmd1 =
{ 0xE0, 15, //Positive gamma control
		{ 0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A,
				0x08, 0x16, 0x1A, 0x0F } };
DISPLAY_cmd cmd2 =
{ 0XE1, 15, //Negative gamma control
		{ 0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E,
				0x0D, 0x35, 0x37, 0x0F } };
const DISPLAY_cmd cmd3 =
{ 0xC0, 2, //Power control 1
		{ 0x17, 0x15 } };
const DISPLAY_cmd cmd4 =
{ 0xC1, 1, //Power control 2
		{ 0x41 } };
const DISPLAY_cmd cmd5 =
{ 0xC5, 3, //VCOM control 1
		{ 0x00, 0x12, 0x80 } };
const DISPLAY_cmd cmd6 =
{ 0x36, 1, //Memory access control
		{ 0x40 } }; // Was 0x48, but it gave BGR, while i want RGB.
const DISPLAY_cmd cmd7 =
{ 0x3A, 1, //Interface pixel format!!!!!!!!!!!!!!!!!!!!!!!
		{ IPFARGUMENT } };
const DISPLAY_cmd cmd8 =
{ 0xB0, 1, //Interface mode control
		{ 0x80 } };
const DISPLAY_cmd cmd9 =
{ 0xB1, 1, //Frame rate control in normal mode/full colors
		{ 0xA0 } };
const DISPLAY_cmd cmd10 =
{ 0xB4, 1, //Display inversion control
		{ 0x02 } };
const DISPLAY_cmd cmd11 =
{ 0xB6, 2, //Display function control
		{ 0x02, 0x02 } };
const DISPLAY_cmd cmd12 =
{ 0XE9, 1, //Set image function
		{ 0x00 } };
const DISPLAY_cmd cmd13 =
{ 0xF7, 4, //Adjust control 3
		{ 0xA9, 0x51, 0x2C, 0x82 } };

const DISPLAY_cmd *const commands[] =
{ &cmd1, &cmd2, &cmd3, &cmd4, &cmd5, &cmd6, &cmd7, &cmd8, &cmd9, &cmd10, &cmd11,
		&cmd12, &cmd13 };

#if DISPLAY_BPP == 3
Color_t makeColor(uint8_t red, uint8_t green, uint8_t blue)
{
	Color_t c =
	{ .r = red, .g = green, .b = blue };
	return c;
}
#elif DISPLAY_BPP == 2
Color_t makeColor(uint8_t red, uint8_t green, uint8_t blue)
{
	Color_t c =
	{ .r = red >> 3, .g = green >> 2, .b = blue >> 3 };
	return c;
}

#endif
void display_SPIStart()
{
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void display_SPIStop()
{
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

static void sendCommand(uint8_t command)
{
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(DISPLAY_SPI, &command, 1, 20);
}

static void sendSingleData(uint8_t data)
{
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(DISPLAY_SPI, &data, 1, 20);
}

static void sendManyData(uint8_t *data, uint16_t length)
{
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(DISPLAY_SPI, data, length, 1000);
}

void sendManyDataAsync(uint8_t *data, uint16_t length)
{
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
	HAL_StatusTypeDef status = HAL_ERROR;
	do
	{
		status = HAL_SPI_Transmit_DMA(DISPLAY_SPI, data, length);
	} while (status != HAL_OK);
}

void display_init()
{
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin,
			GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(150); //Reset the display
	display_SPIStart();
	for (uint8_t i = 0; i < 13; i++)
	{
		sendCommand(commands[i]->cmd);
		sendManyData((uint8_t*) commands[i]->args, commands[i]->argc);
	}
	sendCommand(0x11);
	display_SPIStop();

	HAL_Delay(120);

	display_SPIStart();
	sendCommand(0x29);
	display_SPIStop();
}

void display_setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint8_t dataBuffer[] =
	{ x1 >> 8, x1 & 0xFF, x2 >> 8, x2 & 0xFF };
	sendCommand(0x2A); //CASET
	sendManyData(dataBuffer, 4);

	dataBuffer[0] = y1 >> 8;
	dataBuffer[1] = y1 & 0xFF;
	dataBuffer[2] = y2 >> 8;
	dataBuffer[3] = y2 & 0xFF;
	sendCommand(0x2B); //PASET
	sendManyData(dataBuffer, 4);
	sendCommand(0x2C);     //Write to RAM
}

void display_fillScreen(Color_t color)
{
	display_fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, color);
}

void display_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		Color_t color)
{
	display_startSendingImage(x, y, w, h);
	uint16_t buffersize =
			w < (MAX_BUFFER_SIZE / DISPLAY_BPP) ?
					w : (MAX_BUFFER_SIZE / DISPLAY_BPP);
	Color_t buffer[buffersize];
	uint32_t remainingToSend = w * h;

	for (uint16_t i = 0; i < buffersize; i++)
	{
		buffer[i] = color;
	}

	while (remainingToSend)
	{
		if (buffersize < remainingToSend)
		{
			sendManyData((uint8_t*) buffer, buffersize * DISPLAY_BPP);
			remainingToSend -= buffersize;
		}
		else
		{
			sendManyData((uint8_t*) buffer, remainingToSend * DISPLAY_BPP);
			remainingToSend = 0;
		}
	}
	display_SPIStop();
}

void display_paint_red()
{
	Color_t color = makeColor(200, 0, 0);
	display_fillScreen(color);
}

void display_startSendingImageFullScreen()
{
	display_startSendingImage(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

uint32_t remainingBytes = 0;

void display_startSendingImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
		return;
	if ((x + w - 1) >= DISPLAY_WIDTH)
		w = DISPLAY_WIDTH - x;
	if ((y + h - 1) >= DISPLAY_HEIGHT)
		h = DISPLAY_HEIGHT - y;

	display_SPIStart();
	display_setAddrWindow(x, y, x + w - 1, y + h - 1);
	remainingBytes = w * h * DISPLAY_BPP;
}
void display_sendImagePart(uint8_t *buffer, uint32_t size)
{
	if (size < remainingBytes)
	{
		sendManyData(buffer, size);
		remainingBytes -= size;
	}
	else
	{
		sendManyData(buffer, remainingBytes);
		remainingBytes = 0;
		display_SPIStop();
	}
}

void display_sendImagePartAsync(uint8_t *buffer, uint32_t size)
{
	if (size < remainingBytes)
	{
		sendManyDataAsync(buffer, size);
		remainingBytes -= size;
	}
	else
	{
		sendManyData(buffer, remainingBytes);
		remainingBytes = 0;
		display_SPIStop();
	}
}
