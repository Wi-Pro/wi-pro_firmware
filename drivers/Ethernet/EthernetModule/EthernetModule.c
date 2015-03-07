/*
 * EthernetModule.c
 *
 * Created: 2/7/2015 5:16:59 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "EthernetLibrary.h"

int main(void){
	
	unsigned char receiveMessage[500];

	unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
	unsigned char local_ip_addr[] = {192,168,1,5};
	unsigned char sub_mask[] = {255,255,255,0};
	unsigned char gtw_addr[] = {192,168,1,1};

	unsigned char server_ip_addr[] = {104,131,36,80};
	unsigned char server_port[] = {00,80};
	unsigned char source_port[] = {00,80};
	
	UARTStream_Init();  
	Ethernet_Init(mac_addr, local_ip_addr, sub_mask, gtw_addr);
	Server_Connect(TCP_MODE, server_ip_addr, server_port, source_port);
	//unsigned char message[100] = "GET /uploads/hex/2222222222/Toggle.hex HTTP/1.1";
	//unsigned char message[] = "GET http://www.wi-pro.us/uploads/hex/11111111111/default.hex HTTP/1.1 \nHost: www.wi-pro.us \n\n";
	unsigned char message[] = "POST /data?t=Hello%20World HTTP/1.1\nhost: www.wi-pro.us\n\n";
	unsigned char* messagePointer = message;

	unsigned char* recMessagePointer = receiveMessage;

	printf("Sent Message: %s\n", message);
	SendData(messagePointer, strlen((char *)messagePointer));
	ReceiveData(recMessagePointer, 100);
	printf("Receive Size: %d\n", ReceiveSize());
	printf("Message: %s", recMessagePointer);
	SocketDisconnect();
	
	while(1)
	{
	}
}