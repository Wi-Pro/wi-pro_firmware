/*
 * FPGA.c
 *
 * Created: 4/13/2015 8:52:26 PM
 *  Author: Brandon
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include "FPGA.h"

void SPI_FPGA_Init(void)
{
	FPGA_CS_DDR |= (1<<FPGA_SPI_CS);
	FPGA_CS_PORT |= (1<<FPGA_SPI_CS);
	
	WR_DDR |= 1<<FPGAWR;
	WR_PORT &= ~(1<<FPGAWR);
	SPI_FPGA_Write(FPGA_Disable);
}

void SPI_FPGA_Write(unsigned char SPI_Data)
{
	FPGA_CS_PORT &= ~(1<<FPGA_SPI_CS);
	// Start Write transmission
	SPDR = SPI_Data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	FPGA_CS_PORT |= (1<<FPGA_SPI_CS);
}