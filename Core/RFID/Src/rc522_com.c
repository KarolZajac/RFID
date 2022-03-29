/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eth.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

uint8_t rc522_registerToByteRead(uint8_t reg)
{
	return ((reg << 1) & 0x7E) | 0x80;
}

uint8_t rc522_registerToByteWrite(uint8_t reg)
{
	return ((reg << 1) & 0x7E);
}

uint8_t rc522_readReg(uint8_t reg)
{
	const uint8_t toWrite = rc522_registerToByteRead(reg);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, &toWrite, 1, 100);
	uint8_t returned = 0;
	HAL_SPI_Receive(&hspi3, &returned, 1, 100);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
	return returned;
}

void rc522_writeReg(uint8_t reg, uint8_t val){

	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	uint8_t data[2] = {rc522_registerToByteWrite(reg), val};
	HAL_SPI_Transmit(&hspi3, data, 2, 100);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
}
