/*
 * card_com.h
 *
 *  Created on: 8 May 2022
 *      Author: Marcin
 */

#ifndef RFID_INC_CARD_COM_H_
#define RFID_INC_CARD_COM_H_

#include "inttypes.h"
uint8_t card_read(uint8_t blockId, uint8_t *buffer, uint8_t *bufferSize);

uint8_t card_authenticate(uint8_t *cardID, uint8_t *key,uint8_t blockId);
void card_stopCrypto();

uint8_t card_select(uint8_t *cardID);


#endif /* RFID_INC_CARD_COM_H_ */
