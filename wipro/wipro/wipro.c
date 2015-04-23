/*
 * wipro.c
 *
 * Created: 4/17/2015 12:55:14 AM
 *  Author: Brandon
 */ 
#define F_CPU 8000000UL 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "Program/ProgramDriver.h"
#include "Drivers/RAM/MemoryMap.h"
#include "Drivers/Ethernet/Ethernet.h"
#include "Program/AVR.h"

int main(void)
{
	_delay_ms(1000);
	connectionInit(); 
	hexInit();
	//ethernetInit(); 
	sei(); 
	//Collecting garbage transmission on wi-fi module bootup 
	//receiveStatus();
	//networkConnect("UNOGuest", ""); 
	//_delay_ms(100);
	//networkTest();  
	//pollingInit(); 
	getFlagStatus(); 
	//writeHexFileTest(); 
	//getHexFile();
	//cli(); 
	//Program(ATtiny2313);
	//setMachineMode();
	//_delay_ms(2000);
	//if(serverConnect("www.wi-pro.us", "80"))
	//{
		//printf("Connected!!\n");
	//}  
	//else
	//{
		//printf("Connection Failed\n"); 
	//}
	//pollingInit(); 
	
	//printf("\n\n");
	//writeHexFileTest();
	//RAMPrint(HEX_FILE_ADDRESS, 50);
	//Write test file to external RAM
	//Test switching circuitry code
	
	//Program(ATmega324PA);
	
	printf("Done!\n"); 
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

