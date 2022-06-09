/*
 * bmpLoader.h
 *
 *  Created on: 9 Jun 2022
 *      Author: Marcin
 */

#ifndef INC_BMPLOADER_H_
#define INC_BMPLOADER_H_
#include "stdint.h"

unsigned loadPixelBufferFromBmpFile(uint8_t *buffer, uint16_t bufferSize, const char* filename);

#endif /* INC_BMPLOADER_H_ */
