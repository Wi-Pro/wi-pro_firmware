/*
 * SPIDriver.c
 *
 * Created: 3/1/2015 3:44:11 PM
 *  Author: Brandon
 */ 

#include <stdio.h>
#include <avr/io.h>
#include "SPIDriver.h"

void SPI_Init()
{
	// Set MOSI ,SCK, and SS as output, others as input
	SPI_DDR = (1<<MOSI)|(1<<SCK)|(1<<SS);
	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
	// Enable SPI, Master Mode 0, set the clock rate fck/2
	SPCR0 = (1<<SPE0)|(1<<MSTR0);
}

void SPI_Write(unsigned int addr,unsigned char data, unsigned char writeOpcode)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<SPI_CS);
	// Start Wiznet W5100 Write OpCode transmission
	SPDR0 = writeOpcode;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR0 = (addr & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = addr & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// Start Data transmission
	SPDR0 = data;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
}

unsigned char SPI_Read(unsigned int addr, unsigned char readOpcode)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<SPI_CS);
	// Start Wiznet W5100 Read OpCode transmission
	SPDR0 = readOpcode;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR0 = (addr & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = addr & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// Send Dummy transmission for reading the data
	SPDR0 = 0x00;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
	return(SPDR0);
}