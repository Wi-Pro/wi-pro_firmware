/*
 * wipro.c
 *
 * Created: 4/17/2015 12:55:14 AM
 *  Author: Brandon
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include "Program/ProgramDriver.h"
#include "Drivers/RAM/MemoryMap.h"
#include "Program/AVR.h"

int main(void)
{
	char* SignatureBytes;
	connectionInit(); 
	hexInit();
	//setMachineMode();
	_delay_ms(1000);
	if(serverConnect("www.wi-pro.us", "80"))
	{
		printf("Connected!!\n");
	}  
	else
	{
		printf("Connection Failed\n"); 
	}
	//pollingInit(); 
	//printf("\n\n");
	//writeHexFileTest();
	//RAMPrint(HEX_FILE_ADDRESS, 50);
	//Write test file to external RAM
	//Test switching circuitry code
	
	//SignatureBytes = ReadSignatureBytes();
	
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
	
	printf("Done!\n"); 
    while(1)
    {
        //TODO:: Please write your application code 
    }
}