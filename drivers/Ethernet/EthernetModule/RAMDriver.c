/*
 * RAMDriver.c
 *
 * Created: 3/25/2015 8:38:56 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAMDriver.h"


void SPI_Init()
{
	// Set MOSI ,SCK, and SS as output, others as input
	SPI_DDR = (1<<MOSI)|(1<<SCK)|(1<<SS);
	// CS pin is not active
	RAM_DDR |= (1<<RAM_CS);
	// Enable SPI, Master Mode 0, set the clock rate fck/4
	SPCR0 = (1<<SPE0)|(1<<MSTR0);
}

int getRAMStatus()
{
	//Enable RAM CS
	RAM_PORT &= ~(1<<RAM_CS);
	SPDR0 = RDSR;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Send Dummy transmission for reading the data
	SPDR0 = 0x00;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	//Disable RAM CS
	RAM_PORT |= (1<<RAM_CS);
	return(SPDR0);
}

void setRAMStatus(char mode)
{
	// Activate the CS pin
	RAM_PORT &= ~(1<<RAM_CS);
	SPDR0 = WRSR;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Data transmission
	SPDR0 = mode;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// CS pin is not active
	RAM_PORT |= (1<<RAM_CS);
}

void SPI_WriteAddress(uint16_t address)
{
	SPDR0 = WRITE;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR0 = (address & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = address & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
}

void SPI_WriteData(char data)
{
	// Start Data transmission
	SPDR0 = data;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
}

void SPI_ReadAddress(uint16_t address)
{
	SPDR0 = READ;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	SPDR0 = (address & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = address & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
}

char SPI_ReadData()
{
	// Send Dummy transmission for reading the data
	SPDR0 = 0x00;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	return(SPDR0);
}

uint16_t RAMWrite(char* data, uint16_t startAddress, uint16_t length)
{
	// Activate the CS pin
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_WriteAddress(startAddress);
	SPI_WriteData(0x00);
	RAM_PORT |= (1<<RAM_CS);
	setRAMStatus(SEQ);
	getRAMStatus();
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_WriteAddress(startAddress);
	for(int i = 0; i < length; i++)
	{
		//SPI_WriteAddress(startAddress + i);
		SPI_WriteData(*(data + i));
	}
	RAM_PORT |= (1<<RAM_CS);
		

	return startAddress; 
}

char* RAMRead(uint16_t startAddress, uint16_t length)
{
	char* data = ""; 
	setRAMStatus(SEQ);
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_ReadAddress(startAddress);
	int i; 
	for(i=0; i<length; i++)
	{
		//printf("Writing %d\n", i);
		//*(data + i) = SPI_ReadData();
		//if(i > 10)
		printf("%c", SPI_ReadData()); 
	}
	RAM_PORT |= (1<<RAM_CS); 
	printf("\n\nPosition: %d", i); 
	
	return data; 
}

void RAMWriteByte(char data, uint16_t address)
{
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_WriteAddress(address);
	SPI_WriteData(data);
	RAM_PORT |= (1<<RAM_CS);
}
