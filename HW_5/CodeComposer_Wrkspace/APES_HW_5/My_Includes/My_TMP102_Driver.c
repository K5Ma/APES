/*
 * My_TMP102_Driver.c
 *
 *  Created on: Apr 10, 2019
 *      Author: Khalid AlAwadhi
 *
 *  # ALL INFO CAN BE FOUND IN THE .h FILE #
 */
#include "My_TMP102_Driver.h"

//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//TivaWare includes
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"

//FreeRTOS includes

//My includes
#include "Global_Defines.h"



void Init_I2C0()
{
	/* Init I2C peripheral */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	/* Init GPIO PORT B as SCL and SDA are on that PORT - NOT NEEDED AS IT IS DONE IN MAIN PinoutSet() */
	//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	/* Configure PINs PB2(SCL) and PB3(SDA) */
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	/* Set the function for these PINs:
	 * Open-drain operation with weak pull-ups */
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	/* Enable and initialize the I2C2 master module.
	 * Use the system clock for the I2C0 module */
	I2CMasterInitExpClk(I2C0_BASE, SYSTEM_CLOCK, USE_400KBPS_DATARATE);
}



void Init_Alert_PIN_Polling()
{
	/* Init PIN PM5 be a GPIO INPUT */
	GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_5);
}



bool Alert_PIN_Poll()
{
	/* Check the value of the PIN */
	if( !(GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5)) )
	{
		return true;
	}
	else
	{
		return false;
	}
}



void I2C_SendByte(uint8_t Slave_Addr, uint8_t Register_Addr, uint8_t Data)
{
	/* Start with WRITE */
	I2CMasterSlaveAddrSet(I2C0_BASE, Slave_Addr, WRITE);

	/* Set the register to send data to */
	I2CMasterDataPut(I2C0_BASE, Register_Addr);

	/* Send wanted register to SLAVE from MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	/* Wait until MASTER module says it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));

	/* Store the data to be sent */
	I2CMasterDataPut(I2C0_BASE, Data);

	/* Send data from MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	/* Wait until MASTER module says it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));
}

/////////////////////////////////////////////////////////////////////////
void I2C_SendTwoBytes(uint8_t Slave_Addr, uint8_t Register_Addr, uint8_t Data1, uint8_t Data2)
{
	/* Start with WRITE */
	I2CMasterSlaveAddrSet(I2C0_BASE, Slave_Addr, WRITE);

	/* Set the register to send data to */
	I2CMasterDataPut(I2C0_BASE, Register_Addr);

	/* Send wanted register to SLAVE from MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	/* Wait until MASTER module says it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));

	/* Store the Data1 to be sent */
	I2CMasterDataPut(I2C0_BASE, Data1);

	/* Send data from MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

	/* Wait until MASTER module says it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));

	/* Store the data to be sent */
	I2CMasterDataPut(I2C0_BASE, Data2);

	/* Send data from MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	/* Wait until MASTER module says it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));
}
/////////////////////////////////////////////////////////////////////////



void TMP102_OpenPointerRegister(uint8_t pointerReg)
{
	/* Start with WRITE */
	I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_ADDR, WRITE);

	/* Set the pointerReg to be sent to the SLAVE in the data register */
	I2CMasterDataPut(I2C0_BASE, pointerReg);

	/* Initiate transfer to SLAVE from the MASTER */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	/* Wait until MASTER module is it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));
}



uint8_t TMP102_ReadRegister(bool RegisterNumber)
{
	/* Register data will be stored here */
	uint8_t RegisterByte[2];

	/* Start with READ */
	I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_ADDR, READ);

	/* Read first byte(MSB): */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

	/* Wait until MASTER module is it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));

	/* Get MSB */
	uint8_t MSB_Data = I2CMasterDataGet(I2C0_BASE);

	/* Done reading MSB */
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

	/* Wait until MASTER module is it's busy. !! Errata I2C#08 !! */
	while(!I2CMasterBusy(I2C0_BASE));
	/* Wait until MASTER is done */
	while(I2CMasterBusy(I2C0_BASE));

	/* Get LSB */
	uint8_t LSB_Data = I2CMasterDataGet(I2C0_BASE);

	RegisterByte[0] = MSB_Data;
	RegisterByte[1] = LSB_Data;

	return RegisterByte[RegisterNumber];
}



void TMP102_Wakeup()
{
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read current Configuration register value */
	uint8_t RegisterByte = TMP102_ReadRegister(0);

	/* Clear ShutDown (SD) BIT 0 of first byte */
	RegisterByte &= 0xFE;

	/* Set the Configuration Register with the updated byte */
	I2C_SendByte(TMP102_ADDR, CONFIG_REGISTER, RegisterByte);
}


void TMP102_Sleep()
{
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read current Configuration register value */
	uint8_t RegisterByte = TMP102_ReadRegister(0);

	/* Set ShutDown (SD) BIT 0 of first byte */
	RegisterByte |= 0x01;

	/* Set the Configuration Register with the updated byte */
	I2C_SendByte(TMP102_ADDR, CONFIG_REGISTER, RegisterByte);
}


float TMP102_ReadTempC()
{
	uint8_t RegisterByte[2]; 			//Will store the bytes from the Temp register
	int16_t RawTempData;  				//Will store the raw temperature stored in TMP102 register

	/* Change pointer address to Temperature register */
	TMP102_OpenPointerRegister(TEMPERATURE_REGISTER);

	/* Read from Temperature register */
	RegisterByte[0] = TMP102_ReadRegister(0);
	RegisterByte[1] = TMP102_ReadRegister(1);

	/* BIT 0 of second byte will always be 0 in 12-bit readings and 1 in 13-bit */
	if(RegisterByte[1] & 0x01)	//13 BIT mode
	{
		/* Combine bytes to create a signed int */
		RawTempData = ( (RegisterByte[0]) << 5 ) | (RegisterByte[1] >> 3);

		/* Temperature data can be + or -, if it is negative,
		 * convert 13 BIT to 16 BIT and use 2s compliment */
		if(RawTempData > 0xFFF)
		{
			RawTempData |= 0xE000;
		}
	}
	else	//12 BIT mode
	{
		/* Combine bytes to create a signed int */
		RawTempData = ((RegisterByte[0]) << 4) | (RegisterByte[1] >> 4);
		/* Temperature data can be + or -, if it is negative,
		 * convert 12 BIT to 16 BIT and use 2s compliment */
		if(RawTempData > 0x7FF)
		{
			RawTempData |= 0xF000;
		}
	}

	/* Convert digital reading to analog temperature (1-BIT is equal to 0.0625 C) */
	return RawTempData*0.0625;
}



void TMP102_SetFault(uint8_t fault)
{
	/* Error handling */
	if(fault > 3)
	{
		fault = 0;
	}

	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read current Configuration register value */
	uint8_t RegisterByte = TMP102_ReadRegister(0);

	/* Load new fault setting */
	RegisterByte &= 0xE7;  				//Clear F 0/1 (BIT 3 and 4 of first byte)
	RegisterByte |= fault << 3;			//Shift new fault setting

	/* Set the Configuration Register with the updated byte */
	I2C_SendByte(TMP102_ADDR, CONFIG_REGISTER, RegisterByte);
}



void TMP102_SetAlertPolarity(bool Polarity)
{
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read current Configuration register value */
	uint8_t RegisterByte = TMP102_ReadRegister(0);

	/* Load new value for polarity */
	RegisterByte &= 0xFB; 				//Clear POL (BIT 2 of RegisterByte)
	RegisterByte |= Polarity << 2;  	//Shift in new POL bit

	/* Set the Configuration Register with the updated byte */
	I2C_SendByte(TMP102_ADDR, CONFIG_REGISTER, RegisterByte);
}



void TMP102_SetAlertMode(bool Mode)
{
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read current Configuration register value */
	uint8_t RegisterByte = TMP102_ReadRegister(0);

	/* Load new Alert mode */
	RegisterByte &= 0xFD;				//Clear old TM BIT (BIT 1 of first byte)
	RegisterByte |= Mode << 1;			//Shift in new TM BIT

	/* Set the Configuration Register with the updated byte */
	I2C_SendByte(TMP102_ADDR, CONFIG_REGISTER, RegisterByte);
}



void TMP102_SetConversionRate(uint8_t ConvR)
{
	uint8_t RegisterByte[2]; 			//Will store the bytes to be sent

	/* Error handling */
	if(ConvR > 3)
	{
		ConvR = 3;
	}

	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read from Configuration register */
	RegisterByte[0] = TMP102_ReadRegister(0);
	RegisterByte[1] = TMP102_ReadRegister(1);

	/*  Load new conversion rate */
	RegisterByte[1] &= 0x3F;  			//Clear CR 0/1 (BIT 6 and 7 of second byte)
	RegisterByte[1] |= ConvR << 6;		//Shift in new conversion rate

	/* Set the Configuration Register with the updated bytes */
	I2C_SendTwoBytes(TMP102_ADDR, CONFIG_REGISTER, RegisterByte[0], RegisterByte[1]);
}



void TMP102_SetExtendedMode(bool Mode)
{
	uint8_t RegisterByte[2]; 			//Will store the bytes to be sent

	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read from Configuration register */
	RegisterByte[0] = TMP102_ReadRegister(0);
	RegisterByte[1] = TMP102_ReadRegister(1);

	/* Load new value for Extended mode */
	RegisterByte[1] &= 0xEF;			//Clear EM (BIT 4 of second byte)
	RegisterByte[1] |= Mode << 4;		//Shift in new Extended mode bit

	/* Set the Configuration Register with the updated bytes */
	I2C_SendTwoBytes(TMP102_ADDR, CONFIG_REGISTER, RegisterByte[0], RegisterByte[1]);
}



void TMP102_SetHighTempC(float T_HIGH_val)
{
	uint8_t RegisterByte[2];			//Will store the bytes to be sent
	bool ExtendedMode;					//Stores the extended mode BIT here (0[12 BIT]: -55C to +128C | 1[13 BIT]: -55C to +150C)

	/* Check if ExtendedMode should be 12 or 13 BITs */
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read from Configuration register */
	RegisterByte[0] = TMP102_ReadRegister(0);
	RegisterByte[1] = TMP102_ReadRegister(1);

	ExtendedMode = ( RegisterByte[1] & 0x10 ) >> 4;

	/* Split temperature into separate bytes */
	if(ExtendedMode)	//13-BIT mode
	{
		/* Error check: Prevent temperature from exceeding 150C */
		if(T_HIGH_val > 150.0f)
		{
			T_HIGH_val = 150.0f;
		}
		if(T_HIGH_val < -55.0)
		{
			T_HIGH_val = -55.0f;
		}

		/* Convert analog T_HIGH_val to raw Temp value */
		T_HIGH_val = T_HIGH_val/0.0625;

		RegisterByte[0] = ( (int)(T_HIGH_val) >> 5 );
		RegisterByte[1] = ( (int)(T_HIGH_val) << 3 );
	}
	else				//12-BIT mode
	{
		/* Error check: Prevent temperature from exceeding 128C */
		if(T_HIGH_val > 128.0f)
		{
			T_HIGH_val = 128.0f;
		}
		if(T_HIGH_val < -55.0)
		{
			T_HIGH_val = -55.0f;
		}

		/* Convert analog T_HIGH_val to raw Temp value */
		T_HIGH_val = T_HIGH_val/0.0625;

		RegisterByte[0] = ( (int)(T_HIGH_val) >> 4 );
		RegisterByte[1] = ( (int)(T_HIGH_val) << 4 );
	}

	/* Set the T_HIGH Register with the updated bytes */
	I2C_SendTwoBytes(TMP102_ADDR, T_HIGH_REGISTER, RegisterByte[0], RegisterByte[1]);
}



void TMP102_SetLowTempC(float T_LOW_val)
{
	uint8_t RegisterByte[2];			//Will store the bytes to be sent
	bool ExtendedMode;					//Stores the extended mode BIT here (0[12 BIT]: -55C to +128C | 1[13 BIT]: -55C to +150C)

	/* Check if ExtendedMode should be 12 or 13 BITs */
	/* Change pointer address to Configuration register */
	TMP102_OpenPointerRegister(CONFIG_REGISTER);

	/* Read from Configuration register */
	RegisterByte[0] = TMP102_ReadRegister(0);
	RegisterByte[1] = TMP102_ReadRegister(1);

	ExtendedMode = ( RegisterByte[1] & 0x10 ) >> 4;

	/* Split temperature into separate bytes */
	if(ExtendedMode)	//13-BIT mode
	{
		/* Error check: Prevent temperature from exceeding 150C */
		if(T_LOW_val > 150.0f)
		{
			T_LOW_val = 150.0f;
		}
		if(T_LOW_val < -55.0)
		{
			T_LOW_val = -55.0f;
		}

		/* Convert analog T_LOW_val to raw Temp value */
		T_LOW_val = T_LOW_val/0.0625;

		RegisterByte[0] = ( (int)(T_LOW_val) >> 5 );
		RegisterByte[1] = ( (int)(T_LOW_val) << 3 );
	}
	else				//12-BIT mode
	{
		/* Error check: Prevent temperature from exceeding 128C */
		if(T_LOW_val > 128.0f)
		{
			T_LOW_val = 128.0f;
		}
		if(T_LOW_val < -55.0)
		{
			T_LOW_val = -55.0f;
		}

		/* Convert analog T_LOW_val to raw Temp value */
		T_LOW_val = T_LOW_val/0.0625;

		RegisterByte[0] = ( (int)(T_LOW_val) >> 4 );
		RegisterByte[1] = ( (int)(T_LOW_val) << 4 );
	}

	/* Set the T_LOW Register with the updated bytes */
	I2C_SendTwoBytes(TMP102_ADDR, T_LOW_REGISTER, RegisterByte[0], RegisterByte[1]);
}
