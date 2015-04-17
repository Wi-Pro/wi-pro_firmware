/*
 * EthernetModule.c
 *
 * Created: 2/7/2015 5:16:59 PM
 *  Author: Brandon
 */ 

#define F_CPU 8000000UL


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "EthernetDriver.h"
#include "Ethernet.h"
#include "RAMDriver.h"

int main(void){
	//unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
	//unsigned char local_ip_addr[] = {192,168,1,5};
	//unsigned char sub_mask[] = {255,255,255,0};
	//unsigned char gtw_addr[] = {192,168,1,1};

	//unsigned char server_ip_addr[] = {104,131,36,80};
	//unsigned char server_port[] = {00,80};
	//unsigned char source_port[] = {00,80};
	
	UARTStream_Init(); 
	SPI_Init();  
	EnableEthernetInterrupt();
	//sei(); 
	printf("Starting Up...\n");
	ethernetInit(); 
	//_delay_ms(5000);
	//getRequestEthernet("http://www.wi-pro.us/uploads/hex/1000000000/hex/split0.hex"); 
	getRequestEthernet("GET http://www.wi-pro.us/uploads/hex/1000000000/flagfile.txt HTTP/1.1 \nHost: www.wi-pro.us \n\n"); 
	//unsigned char* message = "GET http://www.wi-pro.us/uploads/hex/1000000000/default.hex HTTP/1.1 \nHost: www.wi-pro.us \n\n";
	//unsigned char message[] = "POST /data?t=Hello%20World HTTP/1.1\nhost: www.wi-pro.us\n\n";
	
	//printf("Sent Message: %s\n", message);
	//SendData(message, strlen(message));
	ReceiveData(5, 0);
	//_delay_ms(100);
	//printf("Receive Size: %d\n", ReceiveSize());
	//printf("Message: %s", receiveMessage);
	//while(!getReceiveFlag());
	RAMRead(0, 5);
	//SocketDisconnect();
	printf("Done!");
	while(1)
	{
	}
}