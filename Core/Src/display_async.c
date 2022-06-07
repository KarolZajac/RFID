/*
 * display_async.c
 *
 *  Created on: Feb 14, 2021
 *      Author: Marcin
 */

#include "display_async.h"

void sendManyDataAsync(uint8_t *data, uint16_t length);
void DisplayDriver_TransferCompleteCallback();

void touchgfxDisplayDriverTransmitBlock(const uint8_t *pixels, uint16_t x,
		uint16_t y, uint16_t w, uint16_t h)
{
	display_SPIStart();
	display_setAddrWindow(x, y, x + w - 1, y + h - 1);
	sendManyDataAsync((uint8_t*) pixels, w * h * 3);
}

int touchgfxDisplayDriverTransmitActive()
{
	return HAL_GPIO_ReadPin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin)
			== GPIO_PIN_RESET;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	display_SPIStop();
	DisplayDriver_TransferCompleteCallback();
}

