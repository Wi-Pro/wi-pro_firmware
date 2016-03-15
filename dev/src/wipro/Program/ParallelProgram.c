/*
 * ParallelProgram.c
 *
 * Created: 4/13/2015 8:41:16 PM
 *  Author: Brandon
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "ProgramDriver.h"
#include "../Hex/IntelHex.h"
#include "../Print/PrintDriver.h"
#include "../RAM/RAMDriver.h"
#include "../RAM/MemoryMap.h"

//int main(void)
//{
	//char* SignatureBytes;
	//uart2_init();
	//hexInit();
	//
	//SPI_Init();
	//printf("\n\n");
	//writeHexFileTest();
	//RAMPrint(HEX_FILE_ADDRESS, 50);
	////Write test file to external RAM 
	////Test switching circuitry code
	//ProgInit();
	//EnableProgMode(ATtiny2313);
	//SignatureBytes = ReadSignatureBytes();
	//
	//printf("\nSignature Bytes: %02X %02X %02X\n", *SignatureBytes, *(SignatureBytes + 1), *(SignatureBytes + 2));
	//if ( (*SignatureBytes == 0x1E) && (*(SignatureBytes + 1) == 0x91) && (*(SignatureBytes + 2) == 0x0A) )
	//{
		//printf("Programming!\n");
		//ChipErase();
		//printf("Done Erasing!\n");
		//ReadFlash();
		//printf("\n");
		//ProgramFlash();
		//printf("Done Programming!\n");
		//ReadFlash();
		//printf("\nDone!\n\n");
	//}
	//ExitParallelProgrammingMode();
	 //
	////Then test parallel programming code here 
	//
    //while(1)
    //{
        ////TODO:: Please write your application code 
    //}
//}