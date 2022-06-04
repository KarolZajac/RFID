/*
 * card_com.c
 *
 *  Created on: 8 May 2022
 *      Author: Marcin
 */


#include "../Inc/rc522_com.h"
#include "../Inc/card_com.h"


uint8_t card_read(uint8_t blockId, uint8_t *buffer)
{
	buffer[0]=PICC_READ;
	buffer[1] = blockId;

	rc522_calculateCRC(buffer, 2, &buffer[2]);

	unsigned int bufferSize;
	uint8_t status = rc522_toCard(PCD_TRANSCEIVE, buffer, 4, buffer, &bufferSize);

	if ((status != 1) || (bufferSize != 0x90)){
	    status = 0;
	}

	return status;
}

void card_stopCrypto()
{
	rc522_clearRegBitMask(0x08, 0x08);
}

uint8_t card_authenticate(uint8_t *cardID, uint8_t *key, uint8_t blockId){


	// Build command buffer
	uint8_t sendData[12];
	sendData[0] = PICC_AUTHNT_KEY_A;
	sendData[1] = blockId;

	for (uint8_t i = 0; i < 6; i++) {	// 6 key bytes
		sendData[2+i] = key[i];
	}

	for (uint8_t i = 0; i < 4; i++) {	// The last 4 bytes of the UID
		sendData[8+i] = cardID[i];
	}

	unsigned int returnBits;
	uint8_t status = rc522_toCard(PCD_AUTHENT, sendData, 12, sendData, &returnBits);

	if ((status != 1) || (!(rc522_readReg(0x08) & 0x08))){
	    status = 0;
	}

	return status;
}

uint8_t card_select(uint8_t *cardID){

	uint8_t buffer[9];
	uint8_t size;

	buffer[0]= 0x93;
	buffer[1]= 0x70;

	for(int i=2;i<=6;i++){
		buffer[i]= cardID[i-2];
	}

//	uint8_t serNumCheck = 0;
//	for(int i=0;i<4;i++){
//		serNumCheck ^= cardID[i];
//	}
//	buffer[6]= serNumCheck;

	rc522_calculateCRC(buffer, 7, &buffer[7]);
	//rc522_writeReg(0x0D, 0);

	unsigned int returnBits;
	uint8_t status;

	status = rc522_toCard(PCD_TRANSCEIVE, buffer, 9, buffer, &returnBits);

	xprintf("Card selection transmition status: %d \n", status);
	xprintf("Return data size: %d \n", returnBits);

	xprintf("Card select data out: %d \n", buffer[6]);

	if((status == 1) && (returnBits == 0x18)){
		size = buffer[0];
	}
	else{
		return 0;
	}


//	rc522_calculateCRC(&buffer[6], 1, &buffer[2]);
//
//	if(buffer[2]!= buffer[7] || buffer[3] != buffer[8]){
//		xprintf("CRC wrong");
//		return 0;
//	}

	return size;
}



