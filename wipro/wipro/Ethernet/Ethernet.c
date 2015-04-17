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
#include "../ClientController.h"


unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
unsigned char local_ip_addr[] = {192,168,1,5};
unsigned char sub_mask[] = {255,255,255,0};
unsigned char gtw_addr[] = {192,168,1,1};

unsigned char server_ip_addr[] = {104,131,36,80};
unsigned char server_port[] = {00,80};
unsigned char source_port[] = {00,80};

	
char httpBuff[150]; 

//char* serialNumber = "1000000000"; 

//struct network currentNetwork; 


void ethernetInit()
{
	NetworkSetup(mac_addr, local_ip_addr, sub_mask, gtw_addr);
	while(!Server_Connect(TCP_MODE, server_ip_addr, server_port, source_port));
	//8KB Send and Receive Buffers 
	MemoryInit(); 
}

//Sends a filepath and retrieves the data contained in the file 
char* getFileEthernet(char* filepath, int externRAM, uint32_t RAMAddress, uint16_t receiveLength)
{ 
	char* receiveData; 
	strncpy(httpBuff, "GET ", 4);
	strcat(httpBuff, URL); 
	strcat(httpBuff, filepath);
	strncat(httpBuff, " HTTP/1.1 \nHost: www.wi-pro.us \n\n", 40); 
	printf("Sent: %s\n", httpBuff);
	//EnableEthernetInterrupt(); 
	SendData(httpBuff, strlen(httpBuff), 0);
	ReceiveData(RAMAddress, receiveData, receiveLength, 1, externRAM);
	memset(httpBuff, 0x00, 100); 
	return receiveData; 
}

void updateFileEthernet(char* filepath)
{
	//strncpy(httpBuff, "GET ", 4); 
	strcpy(httpBuff, filepath); 
	strncat(httpBuff, " HTTP/1.1 \nHost: www.wi-pro.us \n\n", 40); 
	printf("%s", httpBuff); 
	SendData(httpBuff, strlen(httpBuff), 0);
	memset(httpBuff, 0x00, 100); 
}

void sendRequestEthernet(char* filepath, uint16_t address)
{
	//char* message = "GET http://wi-pro.us/data/updatessids?wiproid=1000000000&ssid=%200%201%20-51%206C:B0:CE:B8:A2:9E%20Wi-Pro%20%201%201%20-45%2020:4E:7F:12:3F:1C%20Daycare HTTP/1.1\nhost: www.wi-pro.us\n\n";
	//SendData(message, strlen(message), 0);
	//strncpy(httpBuff, "POST ", 5);
	//strcat(httpBuff, filepath);
}

//void getHexFile(uint8_t numOfFiles)
//{
	////char* filepath = "http://www.wi-pro.us/uploads/hex/"; 
	////char* filename = "split"; 
	////strcat(filename, serialNumber); 
	//for(int i = 0; i < numOfFiles; i++)
	//{
		////char* fileNum = strcat(filename, (i|0x30));
		////strcat(filename, fileNum);
		////strcat(filename, ".hex"); 
		////getFileEthernet(filepath); 		
	//}
//}

