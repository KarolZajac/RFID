/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eth.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "../Inc/rc522_com.h"

#define RC522_MAX_LEN         16


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

void rc522_setRegBitMask(uint8_t reg, uint8_t mask) {
	rc522_writeReg(reg, rc522_readReg(reg) | mask);
}

void rc522_clearRegBitMask(uint8_t reg, uint8_t mask) {

	uint8_t temp;
	temp = rc522_readReg(reg);
	rc522_writeReg(reg, temp & (~mask));
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

void antennaOff() {
	rc522_clearRegBitMask(0x14, 0x03);
}

uint8_t rc522_request(uint8_t reqMode, uint8_t *tagType) {

	uint8_t status = 0;
	unsigned int backBits;
	rc522_writeReg(0x0D, 0x07);//bit framing register,
	tagType[0] = reqMode;
	//xprintf("status %d ", status);

	status = rc522_toCard(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
	if ((status != 1) || (backBits != 0x10)) {
		status = 0;
	}

	return status;
}

uint8_t rc522_toCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
		uint8_t *backData, unsigned int *backLen) {

	uint8_t status = 0;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	unsigned int i;

	switch (command) {
	case PCD_AUTHENT: {
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case PCD_TRANSCEIVE: {
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}

	rc522_writeReg(0x02, irqEn | 0x80); // interrupt req
	rc522_clearRegBitMask(0x04, 0x80); // clear interrupt req bits
	rc522_setRegBitMask(0x0A, 0x80);//FIFO init

	rc522_writeReg(0x01, 0x00);

	//Writing data to the FIFO
	for (i = 0; i < sendLen; i++) {
		rc522_writeReg(0x09, sendData[i]);//FIFO data register, fill FIFO with data
	}

	//Execute the command
	rc522_writeReg(0x01, command);//register command again, transfer up to 25 bytes from FIFO to internal buffer
	if (command == PCD_TRANSCEIVE) {
		rc522_setRegBitMask(0x0D, 0x80); //start data transmission startSend=1
	}

	//Waiting to receive data to complete
	i = 2000;//timeout
	do {
		n = rc522_readReg(0x04);//read interrupt request bits
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq));//until timeout or expected interrupts are generated

	rc522_clearRegBitMask(0x0D, 0x80);//Bit framing register, startSend=0 - is that even legal? stop transmission?
	//xprintf("toCard timeout counter: %d\n",i);
	if (i != 0) {//if not timeouted
//		xprintf("ErrorReg: %d\n", rc522_readReg(0x06));
		uint8_t errorReg = rc522_readReg(0x06);
		//xprintf("Error reg: %x\n", errorReg);
		if (!(errorReg & 0x1B)) { //if no error in error register on buffer overflow, collision error, parity error or protocol error
			status = 1;
			if (n & irqEn & 0x01) {//if command==transcieve and timeout interrupt active
				status = 0;//fail
			}

			if (command == PCD_TRANSCEIVE) {
				n = rc522_readReg(0x0A);//get number of bytes in FIFO
				lastBits = rc522_readReg(0x0C) & 0x07;//3 lowest bits in controlReg - RxLastBits, number of valid b
				if (lastBits) {//Not whole byte valid
					*backLen = (n - 1) * 8 + lastBits; // save number of valid bits
				} else {
					*backLen = n * 8;//save number of valid bits
				}

				if (n == 0) {//if no bytes in fifo
					n = 1;//why?
				}
				if (n > RC522_MAX_LEN) {//If too many bytes in fifo
					n = RC522_MAX_LEN;
				}

				//Reading the received data in FIFO
				for (i = 0; i < n; i++) {
					uint8_t d = rc522_readReg(0x09);//fifo data register
					backData[i] = d;
				}
			}
		} else {
			printf("error\r\n");
			status = 0;
		}
	}
	//xprintf("status %d\n", status);
	return status;
}

uint8_t rc522_antiColl(uint8_t *serNum) {

	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	unsigned int unLen;
	//for (i = 0; i < 4; i++)
//    printf("Anticoll In %d: 0x%02x\r\n", i, serNum[i]);

	rc522_writeReg(0x0D , 0x00);

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = rc522_toCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	//printf("returned data length: %d\n", unLen/8);

	//for (i = 0; i < 4; i++)
//      printf("Anticoll ToCard %d: 0x%02x\r\n", i, serNum[i]);

	if (status == 1) {
		//Check card serial number
		for (i = 0; i < 4; i++) {
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]) {
			status = 0;
		}
	}
	xprintf("Anticollision status %d\n ", status);

	return status;
}

uint8_t rc522_checkCard(uint8_t *id) {

	uint8_t status = 0;
	status = rc522_request(PICC_REQIDL, id);  //request card status
	if (status == 1) { //if card detected
		//Anti-collision, return card serial number 4 bytes
		xprintf("\nCard detected\n");
		status = rc522_antiColl(id);
	}

	return status;
}

void rc522_calculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData)
{
  uint8_t i, n;

  rc522_clearRegBitMask(0x05, 0x04);     //CRCIrq = 0
  rc522_setRegBitMask(0x0A, 0x80);      //Clear the FIFO pointer

  //Writing data to the FIFO
  for (i = 0; i < len; i++) {
    rc522_writeReg(0x09, *(pIndata+i));
  }
  rc522_writeReg(0x01, PCD_CALCCRC);

  //Wait CRC calculation is complete
  i = 0xFF;
  do {
    n = rc522_readReg(0x05);
    i--;
  } while ((i!=0) && !(n&0x04));      //CRCIrq = 1
  //  xprintf("CRC timeout counter: %d\n",i);

  //Read CRC calculation result
  pOutData[0] = rc522_readReg(0x22);
  pOutData[1] = rc522_readReg(0x21);
}

