/*
 * Ethernet.c
 *
 * Created: 3/28/2015 11:23:14 PM
 *  Author: Brandon
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Ethernet.h"
#include "EthernetDriver.h"

unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
unsigned char local_ip_addr[] = {192,168,1,5};
unsigned char sub_mask[] = {255,255,255,0};
unsigned char gtw_addr[] = {192,168,1,1};

unsigned char server_ip_addr[] = {104,131,36,80};
unsigned char server_port[] = {00,80};
unsigned char source_port[] = {00,80};

char* serialNumber = "1000000000"; 

struct network currentNetwork; 


void ethernetInit()
{
	NetworkSetup(mac_addr, local_ip_addr, sub_mask, gtw_addr);
	Server_Connect(TCP_MODE, server_ip_addr, server_port, source_port);
	//8KB Send and Receive Buffers 
	MemoryInit(); 
}

unsigned char* getRequestEthernet(char* filepath)
{ 
	//char* request = "GET "; 
	//strcat(request, filepath);
	//strcat(request, " HTTP/1.1 \nHost: www.wi-pro.us \n\n"); 
	printf("Sent: %s", filepath);
	EnableEthernetInterrupt(); 
	SendData(filepath, strlen(filepath)); 
}

void postRequestEthernet(char* filepath, char* data)
{
	
}

void getHexFile(uint8_t numOfFiles)
{
	char* filepath = "http://www.wi-pro.us/uploads/hex/"; 
	char* filename = "split"; 
	strcat(filename, serialNumber); 
	for(int i = 0; i < numOfFiles; i++)
	{
		char* fileNum = strcat(filename, (i|0x30));
		strcat(filename, fileNum);
		strcat(filename, ".hex"); 
		getRequestEthernet(filepath); 		
	}
}

