/*
 * Program.c
 *
 * Created: 4/19/2015 7:12:10 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <string.h>
#include "ProgramDriver.h"
#include "AVR.h"
#include "../Drivers/RAM/RAMDriver.h"
#include "../Drivers/RAM/MemoryMap.h"


int Program(uint32_t sigBytes)
{
	ApplyPullDowns();
	ProgInit();
	resetRAMOffset(); 
	EnableProgMode(sigBytes);
	
	if(verifySignature(sigBytes))
	{
		//Continue with Programming operation 
		printf("Starting Programming!\n");
		printf("Erasing...\n");
		ChipErase();
		printf("\nDone Erasing!\n");
		printf("Programming...\n");
		ProgramFlash(sigBytes);
		printf("\nReading Flash\n");
		ReadFlash();
		printf("Verifying...\n");
		if (VerifyFlash())
		{
			printf("Programming Succeeded!\n");
			ExitParallelProgrammingMode();
			return 1;
		} 
		else
		{
			printf("Programming Failed!\n");
			ExitParallelProgrammingMode();
			return 0;
		}
	}
	else
	{
		printf("Bad Signature Byte!\n"); 
		ExitParallelProgrammingMode();
		return 0; 
	}
	
}

int verifySignature(uint32_t sigBytes)
{
	char* SignatureBytes = ReadSignatureBytes();
	
	printf("\nSignature Bytes: %02X %02X %02X\n", *SignatureBytes, *(SignatureBytes + 1), *(SignatureBytes + 2));
	//if ( (*SignatureBytes == 0x1E) && (*(SignatureBytes + 1) == 0x91) && (*(SignatureBytes + 2) == 0x0A) )
	if ( (*SignatureBytes == ( (sigBytes & 0xFF0000)>>16 ) ) && (*(SignatureBytes + 1) == ( (sigBytes & 0x00FF00)>>8 ) ) && (*(SignatureBytes + 2) == ( (sigBytes & 0x0000FF) )) )
	{
		return 1; 
	}
	//Put other chips here as else if statements 
	return 0; 
}

unsigned int selectChip(uint16_t id)
{
	switch(id)
	{
		case ATtiny2313_ID:
			return 0x001E910A; 
		case ATmega324PA_ID: 
			return 0x001E9511; 
		default:
			return -1; 
	}
}


//Converts ASCII to Hex in external RAM 
//Length should be the transmission length given from the Wi-Fi module 
int compressFile(uint16_t length)
{
	uint8_t buff;
	uint8_t upperNibble; 
	uint8_t lowerNibble; 
	uint8_t hexVal; 
	uint16_t j = 0; 
	
	printf("Length: %d\n", length); 
	
	for(int i=0; i<length; i++)
	{
		upperNibble = RAMReadByte(HEX_FILE_ADDRESS + i);
		//If it's a colon, leave it alone as it is the start code 
		if(upperNibble == ':'){
			RAMWriteByte(0x3A, HEX_FILE_ADDRESS + j);
			j++;
			continue;
		}
		//Discard any carriage returns or line feeds 
		else if(upperNibble == '\r' || upperNibble == '\n'){
			//i++; 
			continue; 
		}
		
		lowerNibble = RAMReadByte(HEX_FILE_ADDRESS + 1 + i);
		//if(i<600)
			//printf("%d...Upper: 0x%02x, Lower: 0x%02x\n", i, upperNibble, lowerNibble);
		
		if(upperNibble > 0x39)
			upperNibble -= 0x37; 
		else
			upperNibble &= 0x0F;
			
		upperNibble <<= 4;  
		hexVal = upperNibble; 
		
		if(lowerNibble > 0x39)
			lowerNibble -= 0x37; 
		else
			lowerNibble &= 0x0F;
			
		hexVal |= lowerNibble; 
		//if(i<600)
			//printf("%d...Hex: 0x%02x\n\n", j, hexVal);
		
		//Backfill the already converted ASCII value's addresses with the new hex value 
		RAMWriteByte(hexVal, HEX_FILE_ADDRESS + j); 
		j++;
		//Need to increment by 2, but this will not be the case if there is a ':', 
		//hence why the for loop does not have i+=2 
		i++;  
	}
	
	return 1; 
}