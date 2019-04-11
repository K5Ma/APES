/*
 * Temp_Task.h
 *
 *  Created on: Apr 9, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_TEMP_TASK_H_
#define MY_INCLUDES_TEMP_TASK_H_

#include <stdint.h>



/**************************************************************************************************************
 * USAGE: This function will initialize the Temp task.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t Temp_TaskInit();



/**************************************************************************************************************
 * USAGE: This task will read temperature values from the TMP102 sensor at a frequency of 1Hz and send them to
 * 		  the Logger task.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Temp_Task(void *pvParameters);


#endif /* MY_INCLUDES_TEMP_TASK_H_ */
