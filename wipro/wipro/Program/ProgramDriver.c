/*
 * ATmega1280Test.c
 *
 * Created: 3/23/2015 6:51:16 PM
 *  Author: Adam Vogel
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "ProgramDriver.h"
#include "../Hex/IntelHex.h"
#include "../Drivers/Voltage_Switching/SwitchingCircuitry.h"
#include "../Drivers/Voltage_Switching/VoltageControlDriver.h"
#include "../Drivers/FPGA/FPGA.h"
#include "../Drivers/Print/PrintDriver.h"
#include "AVR.h"


void ProgInit(void)
{
	//SPI_Switching_Circuitry_Init(); 
	SPI_FPGA_Init();
	
	voltageControlInit();
	setVpp(VPP_12V);
	setVcc(VCC_5V);
	setVLogic(VL_5V);
	_delay_ms(5);
	enableVccRegulator();
	enableVLogic();
	_delay_ms(25);
	//Setting up Control lines
	CONTROL_DDR |= ( (1<<XTAL1) | (1<<OE) | (1<<WR) | (1<<BS1_PAGEL) | (1<<XA0) | (1<<XA1_BS2) | (1<<PAGEL) | (1<<BS2));
	RDY_BSY_DDR &= ~(1<<RDY_BSY);
	DATA_DDR = 0xFF;
	
	LED_DDR |= ((1<<LED_Green) | (1<<LED_Yellow) | (1<<LED_Red));
	LED_PORT |= (1<<LED_Green);
}

void ApplyPullDowns(void)
{
	SPI_Switching_Circuitry_Init();
	
	SPI_Switching_Circuitry_Write(0xFF); //Pull Downs
	SPI_Switching_Circuitry_Write(0xFF);
	SPI_Switching_Circuitry_Write(0xFF);
	SPI_Switching_Circuitry_Write(0xFF);
	SPI_Switching_Circuitry_Write(0xFF);
	
	SPI_Switching_Circuitry_Write(0x00); //GND
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	
	SPI_Switching_Circuitry_Write(0x00); //Pull Ups
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	
	SPI_Switching_Circuitry_Write(0x00); //VCC
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	
	SPI_Switching_Circuitry_Write(0x00); //VPP
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	SPI_Switching_Circuitry_Write(0x00);
	
	SR_CNTRL_PORT |= (1<<SRCS);
	_delay_us(20);
	SR_CNTRL_PORT &= ~(1<<SRCS);
}

void LoadCommand(char command)
{
	//A: Load Command "Program Flash"
	CONTROL_PORT |= 1<<XA1_BS2;
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	DATA_PORT = command;
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
}

void WriteWord(uint16_t data)
{
	//C: Load Data Low Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT |= 1<<XA0;
	DATA_PORT = data & 0x00FF; 
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
	
	//D: Load Data High Byte
	CONTROL_PORT |= 1<<BS1_PAGEL;
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT |= 1<<XA0;
	DATA_PORT = ((data & 0xFF00) >> 8); 
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
}

void LoadLowAddress(uint16_t address)
{
	//B: Load Address Low Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	CONTROL_PORT &= ~(1<<BS2);
	DATA_PORT = (address & 0x00FF);
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
}

void LoadHighAddress(uint16_t address)
{
	//F: Load Address High Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS2);
	CONTROL_PORT |= 1<<BS1_PAGEL;
	DATA_PORT = ((address & 0xFF00) >> 8);
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
}

void LatchData(void)
{
	CONTROL_PORT |= (1<<BS1_PAGEL);
	_delay_us(25);
	CONTROL_PORT |= (1<<PAGEL);
	_delay_us(25);
	CONTROL_PORT &= ~(1<<PAGEL);
	_delay_us(25);
}

void ProgramPage(void)
{
	//G: Program Page
	CONTROL_PORT &= ~(1<<BS2);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	CONTROL_PORT &= ~(1<<WR);
	_delay_us(25);
	CONTROL_PORT |= 1<<WR;
	_delay_us(25);
	while(!(RDY_BSY_In & (1<<RDY_BSY)));
}

void EndPageProgramming(void)
{
	//I: End Page Programming
	CONTROL_PORT |= 1<<XA1_BS2;
	CONTROL_PORT &= ~(1<<XA0);
	DATA_PORT = 0x00;
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
}

void EnableProgMode(uint32_t TargetMicrocontroller)
{
	CONTROL_PORT &= ~(1<<XTAL1);
	CONTROL_PORT &= ~(1<<XA1_BS2 | 1<<XA0 | 1<<BS1_PAGEL | 1<<WR | PAGEL);
	DATA_PORT = 0x00;
	CONTROL_PORT = 0x00;
	
	WR_PORT &= ~(1<<FPGAWR);
	FPGA_Write(TargetMicrocontroller);
	
	switch (TargetMicrocontroller)
	{
		case ATtiny2313 :
			setAtTiny2313();
			break;
		case ATmega324PA :
			setAtMega324PA();
			break;
		case 3 :
			break;
		case 4 :
			break;
		default:
			break;
	}
	
	_delay_us(100);
	enableVppRegulator();
	_delay_us(50);
	CONTROL_PORT |= (1<<WR | 1<<OE);
	_delay_us(500);
}

char* ReadSignatureBytes(void)
{
	static char SignatureBytes[3];
	
	//A: Load Command "Read Signature Bytes"
	LoadCommand(READ_SIG_BYTE);
	
	//B: Load Address Low Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	DATA_PORT = 0x00;
	_delay_us(100);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(100);
	
	//Read data
	DATA_DDR = 0;
	WR_PORT |= (1<<FPGAWR);
	CONTROL_PORT &= ~(1<<OE);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	_delay_us(500);
	SignatureBytes[0] = DATA_PIN;
	printf("0x%02X ", SignatureBytes[0]);
	CONTROL_PORT |= 1<<OE;
	WR_PORT &= ~(1<<FPGAWR);
	DATA_DDR = 0xFF;
	_delay_us(100);
	
	//Load Address Low Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	DATA_PORT = 0x01;
	_delay_us(100);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(100);
	
	//Read data
	DATA_DDR = 0;
	WR_PORT |= (1<<FPGAWR);
	CONTROL_PORT &= ~(1<<OE);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	_delay_us(500);
	SignatureBytes[1] = DATA_PIN;
	printf("0x%02X ", SignatureBytes[1]);
	CONTROL_PORT |= 1<<OE;
	WR_PORT &= ~(1<<FPGAWR);
	DATA_DDR = 0xFF;
	_delay_us(100);
	
	//Load Address Low Byte
	CONTROL_PORT &= ~(1<<XA1_BS2);
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	DATA_PORT = 0x02;
	_delay_us(100);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(100);
	
	//Read data
	DATA_DDR = 0;
	WR_PORT |= (1<<FPGAWR);
	CONTROL_PORT &= ~(1<<OE);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	_delay_us(500);
	SignatureBytes[2] = DATA_PIN;
	printf("0x%02X ", SignatureBytes[2]);
	CONTROL_PORT |= 1<<OE;
	WR_PORT &= ~(1<<FPGAWR);
	DATA_DDR = 0xFF;
	DATA_PORT = 0x00;
	
	return SignatureBytes;
}

void ReadFlash(void)
{
	char DataValueIn = 0;
	
	//A: Load Command "Read Flash"
	LoadCommand(READ_FLASH);
	
	for (unsigned int LowAddressByte = 0; LowAddressByte < 16; LowAddressByte++)
	{
		//F: Load Address High Byte
		LoadHighAddress(0x00);
		
		//B: Load Address Low Byte
		LoadLowAddress(LowAddressByte);
		
		//Read data
		DATA_DDR = 0;
		WR_PORT |= (1<<FPGAWR);
		CONTROL_PORT &= ~(1<<OE);
		CONTROL_PORT &= ~(1<<BS1_PAGEL); //Reading flash word low byte
		_delay_us(500);
		DataValueIn = DATA_PIN;
		printf("0x%02X ", DataValueIn);
		CONTROL_PORT |= 1<<BS1_PAGEL; //Reading flash word high byte
		_delay_us(500);
		DataValueIn = DATA_PIN;
		printf("0x%02X ", DataValueIn);
		CONTROL_PORT |= 1<<OE;
		WR_PORT &= ~(1<<FPGAWR);
		DATA_DDR = 0xFF;
		_delay_us(25);
	}
}

int VerifyFlash(void)
{
	resetRAMOffset(); 
	
	char DataValueIn = 0;
	
	char* hexRow;
	uint16_t byteCount;
	uint16_t address;
	uint16_t data;
	//Keep looping until the hexRow is the end of file or we hit the end of a page
	
	LoadCommand(READ_FLASH);
	
	while(1)
	{
		hexRow = getHexRow();
		
		//printf("We're here now\n");
		
		if(hexRow[RECORD_TYPE] == TYPE_END_OF_FILE)
		{
			return 1;
		}
		
		byteCount = (hexRow[BYTE_COUNT]);
		address = hexRow[ADDRESS_H];
		address <<= 8;
		address |= (hexRow[ADDRESS_L]);
		address /= 2;
		
		int j =0;
		for(int i=0; i<byteCount; i+=2)
		{
			LoadHighAddress(address);
			LoadLowAddress(address + j);
			printf("\nAddress: 0x%04X\n",(address + j));
			j++;
			
			//Read data
			DATA_DDR = 0;
			WR_PORT |= (1<<FPGAWR);
			CONTROL_PORT &= ~(1<<OE);
			CONTROL_PORT &= ~(1<<BS1_PAGEL); //Reading flash word low byte
			_delay_us(500);
			DataValueIn = DATA_PIN;
			printf("0x%02X ", DataValueIn);
			if (DataValueIn != hexRow[DATA_BEGIN + i])
			{
				return 0;
			}
			CONTROL_PORT |= 1<<BS1_PAGEL; //Reading flash word high byte
			_delay_us(500);
			DataValueIn = DATA_PIN;
			printf("0x%02X ", DataValueIn);
			if (DataValueIn != hexRow[DATA_BEGIN + i + 1])
			{
				return 0;
			}
			CONTROL_PORT |= 1<<OE;
			WR_PORT &= ~(1<<FPGAWR);
			DATA_DDR = 0xFF;
			_delay_us(25);
		}
	}
}

void ChipErase(void)
{
	CONTROL_PORT |= 1<<XA1_BS2;
	CONTROL_PORT &= ~(1<<XA0);
	CONTROL_PORT &= ~(1<<BS1_PAGEL);
	DATA_PORT = 0x80;
	_delay_us(25);
	CONTROL_PORT |= 1<<XTAL1;
	_delay_us(25);
	CONTROL_PORT &= ~(1<<XTAL1);
	_delay_us(25);
	CONTROL_PORT &= ~(1<<WR);
	_delay_us(25);
	CONTROL_PORT |= 1<<WR;
	_delay_us(25);
	while(!(RDY_BSY_In & (1<<RDY_BSY)));
}

void ProgramFlash(uint32_t sigBytes)
{
	char* hexRow;
	uint16_t byteCount;
	uint16_t address;
	uint16_t data;
	uint32_t totalBytes = 0; 
	uint16_t pageSize;
	int j;
	int i; 
	//Keep looping until the hexRow is the end of file or we hit the end of a page 
	if(sigBytes == ATtiny2313)
	{
		printf("Page size is 16\n");
		pageSize = 16; 
	}
	else if(sigBytes == ATmega324PA)
	{
		pageSize = 64; 
		printf("Page size is 64\n");
	}
	
	LoadCommand(WRITE_FLASH);
	
	while(1)
	{
		hexRow = getHexRow();
		
		//printf("We're here now\n");
		
		if(hexRow[RECORD_TYPE] == TYPE_END_OF_FILE)
		{
			printf("End of File!\n"); 
			break;
		}
					
		byteCount = (hexRow[BYTE_COUNT]);
		address = hexRow[ADDRESS_H];
		address <<= 8;
		address |= (hexRow[ADDRESS_L]);
		address /= 2; 
		printf("\nByteCount: %d, Address: 0x%04x\n", byteCount, address); 
		j =0;
		for(i=0; i<byteCount; i+=2)
		{
			LoadLowAddress(address + j);
			//printf("Address: 0x%04X\n",(address + j));
			data = hexRow[DATA_BEGIN + i + 1];
			data <<= 8;
			data |= hexRow[DATA_BEGIN + i];
			WriteWord(data);
			//printf("Word: 0x%04X\n",(data));
			totalBytes+=i; 
			j++;
			//printf("Total Bytes: %d, i: %d, j: %d\n", totalBytes, i, j);
			LatchData();
		}
		
		//Check to see if we hit the end of a page 
		if(totalBytes%pageSize == 0)
		{
			printf("Full Page Written\n");
			LoadHighAddress(address);
			ProgramPage();
		}
	}
	
	//After reaching end of file, do a final page write in case we did not fill an entire page
	if(totalBytes%pageSize != 0)
	{
		printf("Partial Page Written\n");
		LoadHighAddress(address);
		ProgramPage();
	} 
	printf("Ending Page Programming!\n");
	EndPageProgramming();
}

void ExitParallelProgrammingMode(void)
{
	disableVppRegulator();
	disableVccRegulator();
	disableVLogic();
	
	_delay_ms(5);
	DATA_PORT = 0x00;
	CONTROL_PORT = 0x00;
	_delay_ms(1);
	SPI_FPGA_Write(FPGA_Disable);
	
	SR_CNTRL_PORT &= ~(1<<SR_RESET); //Clearing Max395s and Shift Registers
	_delay_us(20);
	SR_CNTRL_PORT |= (1<<SR_RESET);
	_delay_us(5);
	SR_CNTRL_PORT |= (1<<SRCS);
	_delay_us(20);
	SR_CNTRL_PORT &= ~(1<<SRCS);
	
	SR_CNTRL_PORT |= (1<<SROE);
	
	LED_PORT |= ((1<<LED_Green) | (1<<LED_Yellow) | (1<<LED_Red));
	_delay_ms(1000);
	LED_PORT |= (1<<LED_Green);
	LED_PORT &= ~((1<<LED_Yellow) | (1<<LED_Red));
}
