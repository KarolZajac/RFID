/*
 * touch.h
 *
 *  Created on: Dec 2, 2021
 *      Author: Marcin
 */

#ifndef INC_TOUCH_H_
#define INC_TOUCH_H_
#include "main.h"
#include "spi.h"

#define TOUCH_SPI &hspi1

typedef struct{
	int16_t x,y,z;
} TouchCoord_t;

void touchISR();
void touchTask();
TouchCoord_t getTouchedGfxCoord();

TouchCoord_t waitForTouch();

#endif /* INC_TOUCH_H_ */
