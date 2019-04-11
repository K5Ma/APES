/*
 * Global_Defines.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_GLOBAL_DEFINES_H_
#define MY_INCLUDES_GLOBAL_DEFINES_H_


/*****************************************
 *      System Clock Rate, 120 MHz       *
 *****************************************/
#define SYSTEM_CLOCK    120000000U



/*****************************************
 *      Priorities of Tasks              *
 *****************************************/
#define PRIORITY_LOGGER_TASK			0
#define PRIORITY_TEMP_TASK				0
#define PRIORITY_LED_TASK				0
#define PRIORITY_ALERT_TASK				0


/*****************************************
 *      UART Defines                     *
 *****************************************/
#define UART_PORT						0
#define UART_BAUD						9600


/***************************************
 *  Task Numbering Enum:               *
 *  Used for source and destination    *
 ***************************************/
typedef enum
{
	Main = 1,
	Logger = 2,
	Temp = 3,
	LED = 4,
	Alert = 5
} Sources;


/*****************************************
 *      Message Structure                *
 *****************************************/
typedef struct MsgStruct
{
	uint8_t Source;
	char Msg[100];
	float Value;
} MsgStruct;




#endif /* MY_INCLUDES_GLOBAL_DEFINES_H_ */
