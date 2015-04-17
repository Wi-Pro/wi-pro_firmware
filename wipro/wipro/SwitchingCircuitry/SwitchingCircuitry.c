/*
 * SwitchingCircuitry.c
 *
 * Created: 4/13/2015 9:15:51 PM
 *  Author: Brandon
 */ 

//Shift Register Data
//2D array that will contain the configuration for the chip in the correct order  

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "SwitchingCircuitry.h"
#include "ProgramDriver.h"

char ShiftRegData[PIN_TYPES][SOCKET_SIZE/8]; 


void SPI_Switching_Circuitry_Init()
{
	// Set MOSI ,SCK, and SS as output, others as input
	SPI_DDR |= ((1<<MOSI)|(1<<SCK)|(1<<SS));
	
	// Enable SPI, Master Mode 0, set the clock rate fck/64
	SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR1));
	
	SR_CNTRL_DDR |= ((1<<SR_RESET) | (1<<MAXCS) | (1<<SROE));
	SRCS_DDR |= (1<<SRCS);
	SRCS_PORT &= ~(1<<SRCS);
	SR_CNTRL_PORT |= ((1<<MAXCS) | (1<<SROE));
	SR_CNTRL_PORT &= ~(1<<SR_RESET);
}

void SPI_Switching_Circuitry_Write(unsigned char SPI_Data)
{
	// Start Write transmission
	SPDR = SPI_Data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void SwitchingCircuitryEnable()
{
	SR_CNTRL_PORT &= ~(1<<SR_RESET); //Clearing Max395s and Shift Registers
	_delay_us(25);
	SR_CNTRL_PORT |= (1<<SR_RESET);
	
	SRCS_PORT |= (1<<SRCS);
	_delay_us(20);
	SRCS_PORT &= ~(1<<SRCS);
	
	SR_CNTRL_PORT &= ~(1<<SROE);
}

//Clears the internal buffer
void ClearShiftRegData()
{
	for(int i=0; i<PIN_TYPES; i++)
	{
		for(int j=0; j<SOCKET_SIZE/8; j++)
		{
			ShiftRegData[i][j] = 0x00; 
		}
	}
}


void SetShiftRegData(uint8_t pinType, char data[SOCKET_SIZE/8])
{
	//MSB gets shifted out first 
	for(int i=0; i<SOCKET_SIZE/8; i++)
	{
		ShiftRegData[pinType][i] = *(data + i); 
	}
}

void WriteShiftRegData()
{
	SwitchingCircuitryEnable(); 
	
	SR_CNTRL_PORT &= ~(1<<MAXCS);
	int i = 0; 
	for(int j=0; j<SOCKET_SIZE/8; j++)
	{
		SPI_Switching_Circuitry_Write(ShiftRegData[i][j]);
	}
	SR_CNTRL_PORT |= (1<<MAXCS);
	
	
	for(i=1; i<PIN_TYPES; i++)
	{
		for(int j=0; j<SOCKET_SIZE/8; j++)
		{
			SPI_Switching_Circuitry_Write(ShiftRegData[i][j]); 
		}
	}
	SRCS_PORT |= (1<<SRCS);
	_delay_us(20);
	SRCS_PORT &= ~(1<<SRCS);
}

void setAtTiny2313()
{
	char shiftTempBuff[5] = {0x00, 0x1F, 0xF7, 0xD0, 0x00};
	SetShiftRegData(LOGIC, shiftTempBuff);
	//Modify Values and shift again 
	shiftTempBuff[0] = 0x00;
	shiftTempBuff[1] = 0x00;
	shiftTempBuff[2] = 0x00; //setting GND to ZIF19
	shiftTempBuff[3] = 0x00;
	shiftTempBuff[4] = 0x00;
	SetShiftRegData(PULL_DOWN, shiftTempBuff);
	shiftTempBuff[0] = 0x00;
	shiftTempBuff[1] = 0x00;
	shiftTempBuff[2] = 0x08; //setting GND to ZIF19
	shiftTempBuff[3] = 0x00;
	shiftTempBuff[4] = 0x00;
	SetShiftRegData(GROUND, shiftTempBuff);
	shiftTempBuff[0] = 0x00;
	shiftTempBuff[1] = 0x00;
	shiftTempBuff[2] = 0x00;
	shiftTempBuff[3] = 0x00;
	shiftTempBuff[4] = 0x00;
	SetShiftRegData(PULL_UP, shiftTempBuff);
	shiftTempBuff[0] = 0x00;
	shiftTempBuff[1] = 0x20; //Setting VCC to ZIF29
	shiftTempBuff[2] = 0x00;
	shiftTempBuff[3] = 0x00;
	shiftTempBuff[4] = 0x00;
	SetShiftRegData(VCC, shiftTempBuff);
	shiftTempBuff[0] = 0x00;
	shiftTempBuff[1] = 0x00;
	shiftTempBuff[2] = 0x00;
	shiftTempBuff[3] = 0x04; //Setting VPP to ZIF10
	shiftTempBuff[4] = 0x00;
	SetShiftRegData(VPP, shiftTempBuff);
	
	WriteShiftRegData();
}





