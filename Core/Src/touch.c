/*
 * touch.c
 *
 *  Created on: Dec 2, 2021
 *      Author: Marcin
 */
#include "touch.h"
#include "usart.h"

#define Z_THRESHOLD 400

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case USER_Btn_Pin:
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		break;
	case TOUCH_IRQ_Pin:
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		break;
	default:
		xprintf("Received an unexpected EXTI from %d\n", GPIO_Pin);
	}
}

static volatile uint8_t touchHappened = 0;

void touchISR()
{
	touchHappened = 1;
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

static void startTransmission()
{
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
}

static void stopTransmission()
{
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
}

static void transact16(const uint8_t *commands, int16_t *outBuffer,
		uint8_t amount)
{
	for (int i = 0; i < amount; ++i)
	{
		const uint8_t txData[] =
		{ 0x00, commands[i] };
		uint8_t rxData[2];
		//HAL_UART_Transmit(&huart3, txData, 2, 100);
		HAL_SPI_TransmitReceive(TOUCH_SPI, txData, rxData, 2, 100);
		//HAL_UART_Transmit(&huart3, rxData, 2, 100);
		outBuffer[i] = (int16_t) rxData[1] | (int16_t) rxData[0] << 8;
		//HAL_UART_Transmit(&huart3, &outBuffer[i], 2, 100);
		//HAL_UART_Transmit(&huart3, "\n", 1, 100);
	}
}

static void transmitFirstCommand()
{
	const uint8_t command = 0xB1;
	HAL_SPI_Transmit(TOUCH_SPI, &command, 1, 100);
}

static int transmitReceiveFirstBatch()
{
	transmitFirstCommand();

	const int numOfTransactions = 2;
	const static uint8_t commands[] =
	{ 0xC1, 0x91 };

	int16_t rxData[numOfTransactions];
	transact16(commands, rxData, numOfTransactions);

	int16_t z = (rxData[0] >> 3) + 4095 - (rxData[1] >> 3);
	return z;
}

static void transactSecondBatch(int16_t *data)
{
	static const uint8_t commands[] =
	{ 0x91, 0xD1, 0x91, 0xD1, 0x91 };
	const uint8_t numOfCommands = sizeof(commands) / sizeof(commands[0]);
	int16_t rxData[numOfCommands];
	transact16(commands, rxData, numOfCommands);
	for (int i = 0; i < 4; ++i)
	{
		data[i] = rxData[i + 1] >> 3;
	}
}

static void zeroSecondBatch(int16_t *data)
{
	for (int i = 0; i < 4; ++i)
		data[i] = 0;
}

static void getLastBatch(int16_t *data)
{
	static const uint8_t commands[] =
	{ 0xD0, 0x00 };
	transact16(commands, data, 2);
	for (int i = 0; i < 2; ++i)
		data[i] >>= 3;
}

static int16_t bestTwoAvg(int16_t x, int16_t y, int16_t z)
{
	int16_t da, db, dc;
	int16_t reta = 0;
	if (x > y)
		da = x - y;
	else
		da = y - x;
	if (x > z)
		db = x - z;
	else
		db = z - x;
	if (z > y)
		dc = z - y;
	else
		dc = y - z;

	if (da <= db && da <= dc)
		reta = (x + y) >> 1;
	else if (db <= da && db <= dc)
		reta = (x + z) >> 1;
	else
		reta = (y + z) >> 1; //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

	return (reta);
}

static TouchCoord_t calcRawValues(int z, int16_t *data)
{
	TouchCoord_t ret;
	if (z < 0)
		z = 0;
	ret.z = z;
	if (z > Z_THRESHOLD)
	{
		ret.x = bestTwoAvg(data[0], data[2], data[4]);
		ret.y = bestTwoAvg(data[1], data[3], data[5]);
	}
	else
	{
		ret.x = ret.y = 0;
	}
	return ret;
}

static TouchCoord_t readTouchFromDisplay()
{

	startTransmission();
	int z = transmitReceiveFirstBatch();
	int16_t data[6];
	if (z >= Z_THRESHOLD)
		transactSecondBatch(data);
	else
		zeroSecondBatch(data);
	getLastBatch(&data[4]);
	stopTransmission();
	return calcRawValues(z, data);
}

TouchCoord_t alignTouchCoord(TouchCoord_t raw);

TouchCoord_t getTouchedGfxCoord()
{
	return alignTouchCoord(readTouchFromDisplay());
}

void touchTask()
{
	//if (touchHappened)
	return;
	{
		TouchCoord_t rawCoord = getTouchedGfxCoord();
		if (rawCoord.z >= Z_THRESHOLD)
		{
			xprintf("Received a touch, x=%d, y=%d, z=%d\n", rawCoord.x,
					rawCoord.y, rawCoord.z);
		}
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		touchHappened = 0;
	}
}

TouchCoord_t waitForTouch()
{
	TouchCoord_t ret;
	do
	{
		ret = readTouchFromDisplay();
	} while (ret.z < Z_THRESHOLD);
	return ret;
}

