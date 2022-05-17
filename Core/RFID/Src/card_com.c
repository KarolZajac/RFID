/*
 * card_com.c
 *
 *  Created on: 8 May 2022
 *      Author: Marcin
 */


#include "../Inc/rc522_com.h"
#include "../Inc/card_com.h"

uint8_t card_authenticate(uint8_t *cardID, uint8_t *key, uint8_t blockId){


		// Build command buffer
		uint8_t sendData[12];
		sendData[0] = PICC_AUTHNT_KEY_A;
		sendData[1] = blockId;
		for (uint8_t i = 0; i < 6; i++) {	// 6 key bytes
			sendData[2+i] = key[i];
		}

		for (uint8_t i = 0; i < 4; i++) {				// The last 4 bytes of the UID
			sendData[8+i] = cardID[i];
		}
		return rc522_toCard(PCD_AUTHENT, sendData, 12, 0, 0);
}

uint8_t card_select(uint8_t *cardID){
	uint8_t buffer[9];

	buffer[0]= 0x93;
	buffer[1]= 0x70;
	for(int i=2;i<=5;i++){
		buffer[i]= cardID[i-2];
	}

	uint8_t serNumCheck = 0;
	for(int i=0;i<4;i++){
		serNumCheck ^= cardID[i];
	}
	buffer[6]= serNumCheck;

	rc522_calculateCRC(buffer, 7, &buffer[7]);
	rc522_writeReg(0x0D, 0);

	uint16_t returnDataSize =0;

	uint8_t status = rc522_toCard(PCD_TRANSCEIVE, buffer, 9, &buffer[6], &returnDataSize);

	xprintf("Card selection transmition status: %d \n", status);
	xprintf("Return data size: %d \n", returnDataSize);

	xprintf("Card select data out: %d \n", buffer[6]);

	if(status!=1){
		return status;
	}
	if(returnDataSize!=24){
		return 0;
	}

	rc522_calculateCRC(&buffer[6], 1, &buffer[2]);

	if(buffer[2]!= buffer[7] || buffer[3] != buffer[8]){
		xprintf("CRC wrong");
		return 0;
	}

	return 1;
}



