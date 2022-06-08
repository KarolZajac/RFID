/*
 * stegan.h
 *
 *  Created on: 8 Jun 2022
 *      Author: Marcin
 */

#ifndef INC_STEGAN_H_
#define INC_STEGAN_H_
#include <stdint.h>

unsigned loadTextIntoBuffer(const char* filename, char* buffer, unsigned capacity);
unsigned embedTextIntoData(char* data, unsigned dataSize, const char* text, unsigned textLength);
unsigned recoverTextFromData(const char* data, unsigned dataSize, uint8_t* text, unsigned maxTextSize);

#endif /* INC_STEGAN_H_ */
