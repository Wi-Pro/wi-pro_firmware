/*
 * hwm_spi.c
 *
 * Created: 3/14/2016 11:27:33 PM
 *  Author: Brandon Pfeifer 
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hwm_pub_spi.h"
#include "hwm_pub_ethernet.h"
#include "hwm_pub_ram.h"

void HWM_spi_init()
{
	// Set MOSI ,SCK, and SS as output, others as input
	SPI_DDR |= (1<<MOSI)|(1<<SCK)|(1<<SS)|(1<<HOLD);
	//Set MISO as Input
	//SPI_DDR &= ~(1<<MISO);
	
	SPI_PORT |= (1<<HOLD);
	// CS pin is not active
	RAM_DDR |= (1<<RAM_CS);
	RAM_PORT |= (1<<RAM_CS);
	// Enable SPI, Master Mode 0, set the clock rate fck/8
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
	//BRP FIX: I think this is being sent because the first byte would never come out 
	//for some reason
	//RAMWriteByte(0x32, 0000);
}

int HWM_spi_set_div(uint8_t division)
{
	//Still needs to be worked on!!
	switch(division)
	{
		//case 2:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 4:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 8:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 16:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 32:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 64:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//case 128:
		//SPCR &= ~(1<<SPR10)|(1<<SPR00);
		//SPSR |= (1<<SPI2X0);
		//break;
		//default:
		//return 0;
		//break;
	}
	
	return 1;
}

void HWM_spi_write_addr(uint32_t address)
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

void HWM_spi_write_data(char data)
{
	// Start Data transmission
	SPDR = data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void HWM_spi_read_addr(uint32_t address)
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

char HWM_spi_read_data()
{
	// Send Dummy transmission for reading the data
	SPDR = 0x00;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	return(SPDR);
}
