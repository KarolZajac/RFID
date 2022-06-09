/*
 * rfid_task.c
 *
 *  Created on: 8 Jun 2022
 *      Author: Marcin
 */

#include "../Inc/rfid_task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usb_host.h"
#include "../RFID/Inc/rc522_com.h"
#include "../RFID/Inc/card_com.h"
#include "Message.h"
#include "stegan.h"
#include "fatfs.h"
#include "bmpLoader.h"

#define BUFFER_SIZE 752
#define TEXT_BUFFER_SIZE 189
#define USER_DATA_BLOCK_NUM 47

extern QueueHandle_t toDisplayQueue;
extern ApplicationTypeDef Appli_state;
static uint8_t buffer[BUFFER_SIZE];
static uint8_t textBuffer[TEXT_BUFFER_SIZE];

uint8_t doWrite = 1;

void dummyLoop()
{
	uint8_t rfid_id[16];
	uint8_t cardKeyA[6];

	uint16_t dummyBufferSize = 16;
	uint8_t dummyBuffer[dummyBufferSize];
	uint8_t data1[16];
	uint8_t data2[16];

	for (uint8_t i = 0; i < 6; ++i)
	{
		cardKeyA[i] = 0x0FF;
	}
	for (uint8_t i = 0; i < 16; ++i)
	{
		data1[i] = 255;
		data2[i] = 7;
	}
	while (1)
	{
		if (rc522_checkCard(rfid_id))
		{

			xprintf("\nRFID code is: \r\n 0x%02x 0x%02x 0x%02x 0x%02x\n",
					rfid_id[0], rfid_id[1], rfid_id[2], rfid_id[3]);

			if (card_select(rfid_id) > 0)
			{
				uint8_t status = card_authenticate(rfid_id, cardKeyA, 4);
				xprintf("\nAuth status: %d\n", status);

				if (status == 1)
				{
					status = card_read(4, dummyBuffer);

					xprintf("\nRead status: %d\n", status);
					xprintf("Read %d bytes\n", dummyBufferSize);
					xprintf("Read data:\n");
					for (uint8_t i = 0; i < 16; i += 8)
						xprintf("%d %d %d %d %d %d %d %d\n", dummyBuffer[i],
								dummyBuffer[i + 1], dummyBuffer[i + 2],
								dummyBuffer[i + 3], dummyBuffer[i + 4],
								dummyBuffer[i + 5], dummyBuffer[i + 6],
								dummyBuffer[i + 7]);

					xprintf("Writing data to card\n");
					if (dummyBuffer[0] == 255)
					{
						status = card_write(4, data2);
					}
					else
					{
						status = card_write(4, data1);
					}
					xprintf("\nWrite status: %d\n", status);
					card_stopCrypto();
					//now in next iteration you can read new data
				}
			}
		}
		vTaskDelay(100);
	}
}

void readFromPendriveToBuffer()
{
	if (Appli_state == APPLICATION_READY)
	{
		FIL f;
		if (f_open(&f, "0:/img.bin", FA_READ) != FR_OK)
		{
			xprintf("Failed to open file!\n");
			return;
		}
		xprintf("Opened file!\n");
		int sumDumped = 0;

		char tmpBuffer[16];
		unsigned numread = 0;
		while (f_read(&f, tmpBuffer, 16, &numread) == FR_OK && numread != 0
				&& sumDumped <= BUFFER_SIZE)
		{
			memcpy(&buffer[sumDumped], tmpBuffer, numread);
			sumDumped += numread;
		}
		xprintf("Read %d bytes from file.\n", sumDumped);
		f_close(&f);
	}
}

void preparePayload()
{
	//readFromPendriveToBuffer();
	loadPixelBufferFromBmpFile(buffer, BUFFER_SIZE, "img.bmp");
	unsigned len = loadTextIntoBuffer("tekst.txt", textBuffer,
			TEXT_BUFFER_SIZE - 1);
	textBuffer[len] = 0;
	embedTextIntoData(buffer, BUFFER_SIZE, textBuffer, len+1);
}

void cardTaskLoop()
{
	uint8_t rfid_id[16];
	uint8_t cardKeyA[6];

	for (uint8_t i = 0; i < 6; ++i)
	{
		cardKeyA[i] = 0x0FF;
	}
	for (uint16_t i = 0; i < BUFFER_SIZE; ++i)
	{
		buffer[i] = i / 16;
	}
	for (uint16_t i = 0; i < TEXT_BUFFER_SIZE; ++i)
	{
		textBuffer[i] = 0x00;
	}
	while (1)
	{
		//xprintf("RFID looping!\n");
		if (rc522_checkCard(rfid_id))
		{
			xprintf("\nRFID code is: \r\n 0x%02x 0x%02x 0x%02x 0x%02x\n",
					rfid_id[0], rfid_id[1], rfid_id[2], rfid_id[3]);

			if (card_select(rfid_id) > 0)
			{

				if (doWrite == 1)
				{
					preparePayload();

					xprintf("Writing data to card\n");
					for (uint8_t i = 0; i < USER_DATA_BLOCK_NUM; i++)
					{
						uint8_t status = card_authenticate(rfid_id, cardKeyA,
								map_logical_to_physical_addres(i));
						//xprintf("\nAuth status: %d\n", status);
						if (status == 1)
						{
							status = card_write(
									map_logical_to_physical_addres(i),
									&buffer[16 * i]);
							xprintf("\nWrite status to block %d: %d\n", i,
									status);
						}
					}
					card_stopCrypto();
					snprintf(textBuffer, TEXT_BUFFER_SIZE, "Written to card");
					toDisplayMessage displayData =
					{ .imgData = NULL, .textDat = textBuffer };
					xQueueSend(toDisplayQueue, &displayData, 50);
				}
				else
				{
					for (uint8_t i = 0; i < USER_DATA_BLOCK_NUM; i++)
					{
						uint8_t status = card_authenticate(rfid_id, cardKeyA,
								map_logical_to_physical_addres(i));
						//xprintf("\nAuth status: %d\n", status);
						if (status == 1)
						{
							status = card_read(
									map_logical_to_physical_addres(i),
									&buffer[16 * i]);
							xprintf("\nBlock %d read status: %d\n", i, status);
						}
					}
					card_stopCrypto();
//						xprintf("Read %d bytes\n", BUFFER_SIZE);
//						xprintf("Read data:\n");
//					for (uint16_t i = 0; i < BUFFER_SIZE; i += 8)
//					{
//						xprintf("Address %d: ", i);
//						xprintf("%d %d %d %d %d %d %d %d\n", buffer[i],
//								buffer[i + 1], buffer[i + 2], buffer[i + 3],
//								buffer[i + 4], buffer[i + 5], buffer[i + 6],
//								buffer[i + 7]);
//					}
					//snprintf(textBuffer, TEXT_BUFFER_SIZE, "Reading from card");
					recoverTextFromData(buffer, BUFFER_SIZE, textBuffer,
							TEXT_BUFFER_SIZE);
					toDisplayMessage displayData =
					{ .imgData = buffer, .textDat = textBuffer };
					xQueueSend(toDisplayQueue, &displayData, 50);
				}
			}
		}
		vTaskDelay(250);
	}
}

void startRfidTask(const void *_)
{

	UNUSED(_);
	while (1)
	{
		xprintf("Looping RFID!\n");
		//dummyLoop();
		cardTaskLoop();
		vTaskDelay(2500);
	}

	vTaskDelete(NULL);
}
