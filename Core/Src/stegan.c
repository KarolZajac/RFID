/*
 * stegan.c
 *
 *  Created on: 8 Jun 2022
 *      Author: Marcin
 */

#include "stegan.h"
#include "fatfs.h"
#include "main.h"

static int16_t min(int16_t a, int16_t b)
{
	return a<b?a:b;
}

unsigned loadTextIntoBuffer(const char *filename, char *buffer,
		unsigned capacity)
{
	char fileNameBuffer[32];
	snprintf(fileNameBuffer, 32, "0:/%s", filename);
	FIL f;
	if (f_open(&f, fileNameBuffer, FA_READ) != FR_OK)
	{
		xprintf("Failed to open file!\n");
		return 0;
	}
	xprintf("Opened file!\n");
	int sumDumped = 0;
	uint8_t toRead = min(16, capacity);
	char tmpBuffer[16];
	unsigned numread = 0;

	while (f_read(&f, tmpBuffer, toRead, &numread) == FR_OK && numread != 0)
	{
		memcpy(&buffer[sumDumped], tmpBuffer, numread);
		sumDumped += numread;
		toRead = min(16, capacity - sumDumped);
	}
	xprintf("Read %d bytes from text file.\n", sumDumped);
	f_close(&f);
	return sumDumped;
}

unsigned embedTextIntoData(char *data, unsigned dataSize, const char *text,
		unsigned textLength)
{
	const unsigned toEmbed = min(textLength, dataSize / 4);
	for (unsigned i = 0; i < toEmbed; ++i)
	{
		const unsigned dataIndex = i * 4;
		for (uint8_t j = 0; j < 4; ++j)
		{
			data[dataIndex + j] = (data[dataIndex + j] & 0xFC)
					| ((text[i] >> 2 * j) & 0x03);
		}
	}
	xprintf("Embedded text lenghth:%d\n", toEmbed);
	return toEmbed;
}

unsigned recoverTextFromData(const char *data, unsigned dataSize, uint8_t *text,
		unsigned maxTextSize)
{
 	unsigned index = 0;
	const unsigned limit = min(maxTextSize, dataSize / 4);
	while (index < limit)
	{
		const unsigned dataIndex = index * 4;
		uint16_t workingOn = 0;
		for (uint8_t j = 0; j < 4; ++j)
		{
			workingOn |= (data[dataIndex + j] & 0x03) << 2 * j;
		}
		text[index++] = workingOn;
		if (workingOn == 0)
			break;
	}
	xprintf("Recovered text length: %d\n", index);
	return index;
}
