/*
 * Program.c
 *
 * Created: 4/19/2015 7:12:10 PM
 *  Author: Brandon
 */ 

#include "ProgramDriver.h"
#include "AVR.h"


int Program(uint32_t sigBytes)

{
	ProgInit();
	EnableProgMode(ATtiny2313);
	
	if(verifySignature())
	{
		//Continue with Programming operation 
		printf("Programming!\n");
		ChipErase();
		printf("Done Erasing!\n");
		ReadFlash();
		printf("\n");
		ProgramFlash();
		printf("Done Programming!\n");
		ReadFlash();
		printf("\nDone!\n\n");
		ExitParallelProgrammingMode();
		return 1; 
	}
	else
	{
		printf("Bad Signature Byte!\n"); 
		ExitParallelProgrammingMode();
		return 0; 
	}
	
}

int verifySignature()
{
	char* SignatureBytes = ReadSignatureBytes();
	
	printf("\nSignature Bytes: %02X %02X %02X\n", *SignatureBytes, *(SignatureBytes + 1), *(SignatureBytes + 2));
	//if ( (*SignatureBytes == 0x1E) && (*(SignatureBytes + 1) == 0x91) && (*(SignatureBytes + 2) == 0x0A) )
	if ( (*SignatureBytes == ATtiny2313_1) && (*(SignatureBytes + 1) == ATtiny2313_2) && (*(SignatureBytes + 2) == ATtiny2313_3) )
	{
		return 1; 
	}
	//Put other chips here as else if statements 
	return 0; 
}


//Converts ASCII to Hex in external RAM 
//Length should be the transmission length given from the Wi-Fi module 
int compressFile(uint16_t length)
{
	uint8_t buff;
	uint8_t upperNibble; 
	uint8_t lowerNibble; 
	uint8_t hexVal; 
	uint16_t j; 
	
	for(int i=0; i<length; i++)
	{
		upperNibble = RAMReadByte(HEX_FILE_ADDRESS + i);
		//If it's a colon, leave it alone as it is the start code 
		if(upperNibble == ':'){
			j++;
			continue;
		}
		
		upperNibble &= 0x0F;
		upperNibble <<= upperNibble;  
		hexVal = upperNibble; 
		
		lowerNibble = RAMReadByte(HEX_FILE_ADDRESS + 1 + i);
		lowerNibble &= 0x0F;
		hexVal |= lowerNibble; 
		
		//Backfill the already converted ASCII value's addresses with the new hex value 
		RAMWriteByte(hexVal, HEX_FILE_ADDRESS + j); 
		j++;
		//Need to increment by 2, but this will not be the case if there is a ':', 
		//hence why the for loop does not have i+=2 
		i++;  
	}
}