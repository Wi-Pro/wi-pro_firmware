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
	RAM_PORT |= (1<<RAM_CS);
	
	// Enable SPI, Master Mode 0, set the clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	RAMWriteByte(0x32, 0000);
}

int setSPIClockDiv(uint8_t division)
{
	//Still needs to be worked on!!
	switch(division)
	{
		//case 2:
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00); 
			//SPSR |= (1<<SPI2X0);
		//break; 
		//case 4:
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break; 
		//case 8: 
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break; 
		//case 16: 
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break;
		//case 32: 
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break;
		//case 64:
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break; 
		//case 128:
			//SPCR0 &= ~(1<<SPR10)|(1<<SPR00);
			//SPSR |= (1<<SPI2X0);
		//break; 
		//default:
			//return 0; 
		//break;
	}
	
	return 1; 
}

int getRAMStatus()
{
	//Enable RAM CS
	RAM_PORT &= ~(1<<RAM_CS);
	SPDR = RDSR;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Send Dummy transmission for reading the data
	SPDR = 0x00;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	//Disable RAM CS
	RAM_PORT |= (1<<RAM_CS);
	return(SPDR);
}

void setRAMStatus(char mode)
{
	// Activate the CS pin
	RAM_PORT &= ~(1<<RAM_CS);
	SPDR = WRSR;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Start Data transmission
	SPDR = mode;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// CS pin is not active
	RAM_PORT |= (1<<RAM_CS);
}

void SPI_WriteAddress(uint32_t address)
{
	SPDR = WRITE;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR = (address & 0xFF0000) >> 16;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR = (address & 0x00FF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address & 0x0000FF);
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void SPI_WriteData(char data)
{
	// Start Data transmission
	SPDR = data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void SPI_ReadAddress(uint32_t address)
{
	SPDR = READ;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address & 0xFF0000) >> 16;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR = (address & 0x00FF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address & 0x0000FF);
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

char SPI_ReadData()
{
	// Send Dummy transmission for reading the data
	SPDR = 0x00;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	return(SPDR);
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

char* RAMRead(uint32_t startAddress, uint16_t length, char* buffer)
{
	setRAMStatus(SEQ);
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_ReadAddress(startAddress);
	int i; 
	for(i=0; i<length; i++)
	{
		*(buffer + i) = SPI_ReadData();
	}
	RAM_PORT |= (1<<RAM_CS); 
	//printf("%d\n", buffer);
	return buffer; 
}

void RAMPrint(uint32_t startAddress, uint16_t length)
{
	//char* data = ""; 
	setRAMStatus(SEQ);
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_ReadAddress(startAddress);
	printf("Address: 0x%06x\n", startAddress);
	int i; 
	for(i=0; i<length; i++)
	{
		printf("0x%02X ", SPI_ReadData());
	}
	RAM_PORT |= (1<<RAM_CS); 
	printf("\nSize: %d\n", i); 
}

void RAMWriteByte(char data, uint32_t address)
{
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_WriteAddress(address);
	SPI_WriteData(data);
	RAM_PORT |= (1<<RAM_CS);
}

char RAMReadByte(uint32_t address)
{
	RAM_PORT &= ~(1<<RAM_CS);
	SPI_ReadAddress(address);
	char data = SPI_ReadData();
	RAM_PORT |= (1<<RAM_CS);
	return data; 
}
