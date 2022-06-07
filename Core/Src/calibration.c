/*
 * calibration.c
 *
 *  Created on: 3 Dec 2021
 *      Author: Marcin
 */
#include "calibration.h"
#include "display.h"

TouchCoord_t bottomLeft, topRight;

static int16_t min(int16_t a, int16_t b)
{
	return a<b?a:b;
}

void calibrate_display()
{
	const uint16_t calibrationSquareSize = min(DISPLAY_WIDTH, DISPLAY_HEIGHT)
			/ 16;
	const uint16_t offset = calibrationSquareSize / 2;
	const uint16_t quarterWidth = DISPLAY_WIDTH / 4;
	const uint16_t quarterHeight = DISPLAY_HEIGHT / 4;
	display_fillRect(quarterWidth - offset, quarterHeight - offset, offset,
			offset, makeColor(0, 128, 100));
	bottomLeft = waitForTouch();
	bottomLeft = waitForTouch();
	display_fillRect(quarterWidth - offset, quarterHeight - offset, offset,
			offset, makeColor(0, 0, 0));
	HAL_Delay(500);

	display_fillRect(3 * quarterWidth - offset, 3 * quarterHeight - offset,
			offset, offset, makeColor(0, 128, 100));
	topRight = waitForTouch();
	topRight = waitForTouch();
	display_fillRect(3 * quarterWidth - offset, 3 * quarterHeight - offset,
			offset, offset, makeColor(0, 0, 0));
}

static int16_t map(int16_t val, int16_t fromMin, int16_t fromMax, int16_t toMin,
		int16_t toMax)
{
	return (val - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
}

TouchCoord_t alignTouchCoord(TouchCoord_t raw)
{
	TouchCoord_t ret = {
			.x = map(raw.y, bottomLeft.y, topRight.y, DISPLAY_WIDTH/4, 3*DISPLAY_WIDTH/4),
			.y = map(raw.x, bottomLeft.x, topRight.x, DISPLAY_HEIGHT/4, 3*DISPLAY_HEIGHT/4),
			.z = raw.z
	};
	return ret;
}

