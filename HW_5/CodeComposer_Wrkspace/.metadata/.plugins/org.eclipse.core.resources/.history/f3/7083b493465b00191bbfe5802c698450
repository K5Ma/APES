/*
 * LED_Task.h
 *
 *  Created on: Apr 7, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_LED_TASK_H_
#define MY_INCLUDES_LED_TASK_H_

#include <stdint.h>



/**************************************************************************************************************
 * USAGE: This function will initialize the LED task.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t LED_TaskInit();



/**************************************************************************************************************
 * USAGE: This task toggles LEDs D1 and D2 on the TIVA board at a frequency of 10Hz, and passes a structure
 * 		  containing a time stamp, toggle count and my name to the Logger task using a FreeRTOS mechanism as
 * 		  the homework wanted.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void LED_Task(void *pvParameters);



#endif /* MY_INCLUDES_LED_TASK_H_ */
