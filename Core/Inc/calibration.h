/*
 * calibration.h
 *
 *  Created on: 3 Dec 2021
 *      Author: Marcin
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_
#include "touch.h"

void calibrate_display();
TouchCoord_t alignTouchCoord(TouchCoord_t raw);

#endif /* INC_CALIBRATION_H_ */
