/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eth.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "../Inc/rc522_com.h"

uint8_t rc522_registerToByteRead(uint8_t reg) {
	return ((reg << 1) & 0x7E) | 0x80;
}

uint8_t rc522_registerToByteWrite(uint8_t reg) {
	return ((reg << 1) & 0x7E);
}

void rc522_init(void) {

	//Hard reset
	rc522_reset();

	rc522_writeReg(0x12, 0x00); //TxMode
	rc522_writeReg(0x13, 0x00);	//RxMode
	rc522_writeReg(0x24, 0x26); //ModWIdth

	rc522_writeReg(0x2A, 0x80); //TMode
	rc522_writeReg(0x2B, 0xA9); //TPrescaler
	rc522_writeReg(0x2D, 0xE8); //TReloadLow
	rc522_writeReg(0x2C, 0x03); //TRealoadHigh

	rc522_writeReg(0x15, 0x40); //TxASK
	rc522_writeReg(0x11, 0x3D); //Mode

	//Enable antenna driver TX1 and TX2
	antennaOn();

}

uint8_t rc522_readReg(uint8_t reg) {
	const uint8_t toWrite = rc522_registerToByteRead(reg);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, &toWrite, 1, 100);
	uint8_t returned = 0;
	HAL_SPI_Receive(&hspi3, &returned, 1, 100);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
	return returned;
}

void rc522_writeReg(uint8_t reg, uint8_t val) {

	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	uint8_t data[2] = { rc522_registerToByteWrite(reg), val };
	HAL_SPI_Transmit(&hspi3, data, 2, 100);
	HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
}

void rc522_reset() {

	HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(25);
	HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(25);
}

void antennaOn() {
	uint8_t value = rc522_readReg(0x14);
	if ((value & 0x03) != 0x03) {
		rc522_writeReg(0x14, value | 0x03);
	}
}
