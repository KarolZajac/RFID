/*
 * rfid_task.h
 *
 *  Created on: 8 Jun 2022
 *      Author: Marcin
 */

#ifndef RFID_INC_RFID_TASK_H_
#define RFID_INC_RFID_TASK_H_

void startRfidTask(const void* _);
void dummyLoop();
void cardTaskLoop();
void readFromPendriveToBuffer();


#endif /* RFID_INC_RFID_TASK_H_ */
