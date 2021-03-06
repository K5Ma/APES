/*
 * Alert_Task.h
 *
 *  Created on: Apr 11, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_ALERT_TASK_H_
#define MY_INCLUDES_ALERT_TASK_H_

#include <stdint.h>



/**************************************************************************************************************
 * USAGE: This function will initialize the Alert task.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t Alert_TaskInit();



/**************************************************************************************************************
 * USAGE: This task will alert the Logger Task if the Temp is above the set threshold.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Alert_Task(void *pvParameters);


#endif /* MY_INCLUDES_ALERT_TASK_H_ */
