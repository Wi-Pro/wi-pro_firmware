/*
 * IntelHex.c
 *
 * Created: 4/13/2015 8:59:22 PM
 *  Author: Brandon
 * Description: Pulls the Hex file from External RAM, row by row, and parses it in accordance with Intel Hex file standards. 
 * For use with the Atmel AVR Microcontrollers 
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "../Drivers/RAM/RAMDriver.h"
#include "../Drivers/RAM/MemoryMap.h"
#include "../Hex/IntelHex.h"


char hexRow[50];
uint16_t RAMOffset; 

//char buffer[60] = {':', 0x10, 0x00, 0x00, 0x00, 0x14, 0xC0, 0x19, 0xC0, 0x18, 0xC0, 0x17, 0xC0, 0x16, 0xC0, 0x15, 0xC0, 0x14, 0xC0, 0x13, 0xC0, 0x42,
				   //':', 0x10, 0x00, 0x10, 0x00, 0x12, 0xC0, 0x11, 0xC0, 0x10, 0xC0, 0x0F, 0xC0, 0x0E, 0xC0, 0x0D, 0xC0, 0x0C, 0xC0, 0x0B, 0xC0, 0x6C,
				   //':', 0x00, 0x00, 0x00, 0x01, 0xFF};

void hexInit()
{
	RAMOffset = 0; 
	memset(hexRow, 0x00, 50); 
} 

void resetRAMOffset()
{
	RAMOffset = 0; 
}

char* getHexRow()
{
	char rowBuff; 
	int i=0; 
	printf("Getting Row!\n"); 
	for(i=0; i<DATA_BEGIN; i++)
	{
		//Read the bytes from RAM and throw them into the buffer
		rowBuff = RAMReadByte(HEX_FILE_ADDRESS + RAMOffset);
		printf("0x%02X ", rowBuff);
		hexRow[i] = rowBuff; 
		RAMOffset++; 
	}
	
	uint8_t dataLength = hexRow[BYTE_COUNT];
	printf("Data Length: %d\n", dataLength); 
	//Add 1 to dataLength to catch the checkSum 
	for(i=0;i<dataLength+1;i++)
	{
		rowBuff = RAMReadByte(HEX_FILE_ADDRESS + RAMOffset);
		printf("0x%02X ",rowBuff);
		hexRow[DATA_BEGIN + i] = RAMReadByte(HEX_FILE_ADDRESS + RAMOffset);
		RAMOffset++; 
	}
	
	return hexRow; 
}

//void writeHexFileTest()
//{	
	//for (int i = 0; i<50; i++)
	//{
		//printf("0x%02X ", *(buffer + i));
	//}
	//printf("\n");
				//
	//RAMWrite(buffer, HEX_FILE_ADDRESS, 50);
					//
//}