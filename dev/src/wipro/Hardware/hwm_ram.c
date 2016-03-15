/*
 * hwm_ram.c
 *
 * Created: 3/25/2015 8:38:56 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hwm_pub_ram.h"
#include "hwm_pub_spi.h"



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



uint16_t RAMWrite(char* data, uint32_t startAddress, uint16_t length)
{
	// Activate the CS pin
	RAM_PORT &= ~(1<<RAM_CS);
	HWM_spi_write_addr(startAddress);
	HWM_spi_write_data(0x00);
	RAM_PORT |= (1<<RAM_CS);
	setRAMStatus(SEQ);
	getRAMStatus();
	RAM_PORT &= ~(1<<RAM_CS);
	HWM_spi_write_addr(startAddress);
	for(int i = 0; i < length; i++)
	{
		//HWM_spi_write_addr(startAddress + i);
		HWM_spi_write_data(*(data + i));
	}
	RAM_PORT |= (1<<RAM_CS);
		

	return startAddress; 
}

char* RAMRead(uint32_t startAddress, uint16_t length, char* buffer)
{
	setRAMStatus(SEQ);
	RAM_PORT &= ~(1<<RAM_CS);
	HWM_spi_read_addr(startAddress);
	int i; 
	for(i=0; i<length; i++)
	{
		*(buffer + i) = HWM_spi_read_data();
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
	HWM_spi_read_addr(startAddress);
	printf("Address: 0x%06x\n", startAddress);
	int i; 
	for(i=0; i<length; i++)
	{
		printf("0x%02X ", HWM_spi_read_data());
	}
	RAM_PORT |= (1<<RAM_CS); 
	printf("\nSize: %d\n", i); 
}

void RAMWriteByte(char data, uint32_t address)
{
	RAM_PORT &= ~(1<<RAM_CS);
	HWM_spi_write_addr(address);
	HWM_spi_write_data(data);
	RAM_PORT |= (1<<RAM_CS);
}

char RAMReadByte(uint32_t address)
{
	RAM_PORT &= ~(1<<RAM_CS);
	HWM_spi_read_addr(address);
	char data = HWM_spi_read_data();
	RAM_PORT |= (1<<RAM_CS);
	return data; 
}
