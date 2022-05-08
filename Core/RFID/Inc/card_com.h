/*
 * card_com.h
 *
 *  Created on: 8 May 2022
 *      Author: Marcin
 */

#ifndef RFID_INC_CARD_COM_H_
#define RFID_INC_CARD_COM_H_

#include "inttypes.h"

uint8_t card_authenticate(uint8_t *cardID, uint8_t *key);

uint8_t card_select(uint8_t *cardID);

#endif /* RFID_INC_CARD_COM_H_ */
