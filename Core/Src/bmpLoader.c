/*
 * bmpLoader.c
 *
 *  Created on: 9 Jun 2022
 *      Author: Marcin
 */
#include "bmpLoader.h"
#include "main.h"
#include "fatfs.h"

void readFile(FIL *f, uint8_t *buffer, uint16_t len)
{
	unsigned read = 0;
	f_read(f, buffer, len, &read);
}

void seekFile(FIL *F, uint16_t offset)
{
	f_lseek(F, offset);
}

void getDimensions(FIL *fileHandle, uint32_t *width, uint32_t *height)
{
	uint8_t data[50];
	seekFile(fileHandle, 0);
	readFile(fileHandle, data, 26); //read first part of header.

	*width = data[18] | (data[19] << 8) | (data[20] << 16) | (data[21] << 24);
	*height = data[22] | (data[23] << 8) | (data[24] << 16) | (data[25] << 24);
}

void loadWholeFile(uint8_t *buffer, FIL *fileHandle)
{
	uint8_t data[50];
	seekFile(fileHandle, 0);
	readFile(fileHandle, data, 26); //read first part of header.

	const uint32_t offset = data[10] | (data[11] << 8) | (data[12] << 16)
			| (data[13] << 24);
	const uint32_t width = data[18] | (data[19] << 8) | (data[20] << 16)
			| (data[21] << 24);
	const uint32_t height = data[22] | (data[23] << 8) | (data[24] << 16)
			| (data[25] << 24);

	readFile(fileHandle, data, offset - 26); //read rest of header.

	const uint32_t rowpadding = (4 - ((width * 3) % 4)) % 4;

	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			if (x % 10 == 0) //read data every 10 pixels = 30 bytes
			{
				if (x + 10 <= width) //read 10
				{
					readFile(fileHandle, data, 10 * 3); //10 pixels
				}
				else
				{
					readFile(fileHandle, data, (width - x) * 3 + rowpadding); //rest of line
				}
			}
			//insert pixel, if within dynamic bitmap boundaries
			if (x < width && ((height - y - 1) < height))
			{

				//24 bit framebuffer
				const uint32_t inx = 3 * (x + (height - y - 1) * width);
				buffer[inx + 0] = data[(x % 10) * 3 + 0];
				buffer[inx + 1] = data[(x % 10) * 3 + 1];
				buffer[inx + 2] = data[(x % 10) * 3 + 2];
			}
		}
	}
}

unsigned loadPixelBufferFromBmpFile(uint8_t *buffer, uint16_t bufferSize,
		const char *filename)
{
	if (bufferSize <= 15 * 15 * 3)
	{
		xprintf("Insufficient buffer size\n");
		return 0;
	}
	char fileNameBuffer[32];
	snprintf(fileNameBuffer, 32, "0:/%s", filename);
	FIL f;
	if (f_open(&f, fileNameBuffer, FA_READ) != FR_OK)
	{
		xprintf("Failed to open BMP file\n");
		return 0;
	}
	uint32_t width, height;
	getDimensions(&f, &width, &height);
	if (width != 15 && height != 15)
	{
		xprintf("Wrong image format!\n");
		return 0;
	}

	loadWholeFile(buffer, &f);
	f_close(&f);
	return 1;
}
