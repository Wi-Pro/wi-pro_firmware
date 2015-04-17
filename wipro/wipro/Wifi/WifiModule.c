/*
 * WifiModule.c
 *
 * Created: 3/7/2015 5:09:19 PM
 *  Author: Brandon
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "PrintDriver.h"
#include "WifiDriver.h"
#include "Wifi.h"
#include "RAMDriver.h"


int main(void)
{
	//cli(); 
	uart_init();
	uart0_init(); 
	//printf("%s\n", string);
	sei(); 
	SPI_Init(); 
	//char* hello = "Hello!";
	//RAMWrite(hello, 0x0001, 6);
	//_delay_ms(10);
	//RAMRead(0x0001, 6);
	//printf("Sending!\n");
	//wifiInit(); 
	setMachineMode();
	//sendCommand(GET, "wlan", NOVAL); 
	//uart_send("scan\r\n\0", 7);
	//char* data = getReceiveBuffer();
	//networkScan();
	//setTestPrint(); 
	//setRAMStatus(BYTE);
	//RAMWriteByte(0x00, 0x0000); 
	//char* networks = networkScan(); 
	//printf("Length: %d\n", strlen(networks));
	//printf("Done Scanning!\n");
	//printf("Networks: \n");
	//for(int j = 0; j<strlen(networks); j++)
	//{
		//printf("%c", networks[j]);
	//}

	//printf("\n");
	//RAMWrite(networks, 0x0000, 20);
	//RAMRead(0x0000, 20);
	//because fuck encryption
	//printf("Begin Main\n");
	//networkConnect("Wi-Pro", "brightshoe902");
	//_delay_ms(3100);
	//serverConnect("www.wi-pro.us", "80"); 
	//setHumanMode();
	//char* data = getReceiveBuffer(); 
	//int length = getTransmissionLength();
	//printf("Length: %d", length);
	//RAMWrite(data, 0x0000, length);
	//RAMRead(0x0000, length);
	//for(int i = 0; i < length; i++)
	//{
		//printf("%c", networks[i]);
	//}
	//printf("Found Receive!\n");
	//printf("Received Data: %s\n", data);
	//int length = strlen(data);
	//printf("Message: ");
	printf("\nDone!\n");
}


