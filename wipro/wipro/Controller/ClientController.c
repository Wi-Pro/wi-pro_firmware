/*
 * ClientController.c
 *
 * Created: 4/6/2015 1:37:42 AM
 *  Author: Brandon
 * Description: This Function will be used to handle application-specific commands over Ethernet and Wi-Fi to and from the server 
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "ClientController.h"
#include "Ethernet/Ethernet.h"
#include "Ethernet/EthernetDriver.h"
#include "Print/PrintDriver.h"
#include "RAM/RAMDriver.h"
#include "Wifi/Wifi.h"
#include "RAM/MemoryMap.h"

uint8_t Flags[FLAG_ARRAY_LENGTH]; 
char filepath[100]; 

void flagInit()
{
	//Clear all flags 
	for(int i = 0; i < FLAG_ARRAY_LENGTH; i++)
	{
		Flags[i] = 0; 
	}
}

void pollingInit()
{
	//Set a 64 prescaler to achieve a ping roughly every half second
	//TCCR1B = (1<<CS11)|(1<<CS10);
	TCCR1B |= (1<<CS12); 
	//Load internal counter registers with min value 
	TCNT1L = 0x00; 
	TCNT1H = 0x00; 
	//Set interrupt to invoke on overflow
	TIMSK1 = (1 << TOIE1);
}

//Checks to see if Ethernet or Wi-Fi is available 
int checkNetworkStatus()
{
	
}

int clearFlags()
{
	if(Flags[ETHERNET])
	{
		strcpy(filepath, GET);
		strcat(filepath, URL);
		strcat(filepath, CLEAR_FLAG);
		strcat(filepath, WIPRO_ID);
		//printf("Clear: %s\n", filepath);
		updateFileEthernet(filepath);
	}
	else if(Flags[WIFI])
	{
		strcpy(filepath, HTTP_GET);
		strcat(filepath, URL); 
		strcat(filepath, CLEAR_FLAG);
		strcat(filepath, WIPRO_ID);
		updateFileWifi(filepath);	
	}
	
	memset(filepath, 0x00, 100);
	return 1; 
}

void sendAvailableNetworks()
{
	networkScan();
	uint16_t tranLength = getTransmissionLength();
	printf("Tran Length: %d\n", tranLength);
	//RAMPrint(WIFI_RAW_ADDRESS, tranLength);
	//_delay_ms(1000);
	//Build file path
	strcpy(filepath, GET);
	strcat(filepath, URL);
	strcat(filepath, SSID_UPDATE);
	strcat(filepath, WIPRO_ID);
	strcat(filepath, SSID_ID);
	printf("filepath: %s", filepath); 
	//Convert List of Networks to Query String
	uint16_t qLength = networkQueryString(filepath);
	//printf("Query Length: %d\n", qLength);
	RAMPrint(WIFI_QSTRING_ADDRESS, qLength);
	//RAMRead(WIFI_QSTRING_ADDRESS, qLength, buffer);
	SendData(WIFI_QSTRING_ADDRESS, qLength, 1);
	memset(filepath, 0x00, 100);
	printf("Sent!");
}

int getHexFile()
{
	Flags[WIFI] = 1; 
	if(Flags[WIFI])
	{
		strcpy(filepath, URL);
		strcat(filepath, HEX_FILE);
		//setTestPrint(1);
		printf("Compress Flag Set!");
		setCompressFlag(1);
		getFileWifi(filepath, 1, HEX_FILE_ADDRESS, 1);
		//setCompressFlag(0); 
		RAMPrint(HEX_FILE_ADDRESS, 1045);
		//printf("Hex File Downloaded!\n");
	}
	
	return 1; 
}

int getFlagStatus()
{
	//Testing 
	TIMSK1 &= ~(1 << TOIE1);
	Flags[ETHERNET] = 0; 
	Flags[WIFI] = 1; 
	
	if(Flags[ETHERNET]){
		getFileEthernet(FLAG_FILE, 1, STATUS_FLAG_ADDRESS, 3); 
		printf("Ram Print: ");
		RAMPrint(STATUS_FLAG_ADDRESS-10, 20);
		printf("\n");
		//RAMRead(STATUS_FLAG_ADDRESS, 1, Flags[PROGRAM]); 
		//RAMRead(STATUS_FLAG_ADDRESS + 1, 1, Flags[PROGRAM + 1]); 
		//RAMRead(STATUS_FLAG_ADDRESS + 2, 1, Flags[PROGRAM + 2]); 
		
		Flags[PROGRAM] = (RAMReadByte(STATUS_FLAG_ADDRESS) & 0x0F);
		Flags[NETWORK_SCAN] = (RAMReadByte(STATUS_FLAG_ADDRESS + 1) & 0x0F);
		Flags[NETWORK_CONNECT] = (RAMReadByte(STATUS_FLAG_ADDRESS + 2) & 0x0F); 
		//printf("Flag Status: %s\n", flags);  
	}
	else if(Flags[WIFI]){
		strcpy(filepath, URL);
		strcat(filepath, FLAG_FILE); 
		//setTestPrint(1);
		getFileWifi(filepath, 1, STATUS_FLAG_ADDRESS, 0); 
		printf("Ram Print: ");
		RAMPrint(STATUS_FLAG_ADDRESS, 3);
		printf("\n");
		Flags[PROGRAM] = (RAMReadByte(STATUS_FLAG_ADDRESS) & 0x0F);
		Flags[NETWORK_SCAN] = (RAMReadByte(STATUS_FLAG_ADDRESS + 1) & 0x0F);
		Flags[NETWORK_CONNECT] = (RAMReadByte(STATUS_FLAG_ADDRESS + 2) & 0x0F);
		
	}
	memset(filepath, 0x00, 100); 
	//Perform actions based on flags 
	if(Flags[PROGRAM] == 0x01)
	{
		//Program Function 
		//printf("Program!\n");
		//PORTD &= ~(1<<CTS);
		//wifiDriverInit(); 
		//PORTD |= (1<<CTS); 
		//_delay_ms(2000); 
		getHexFile(); 
		//printf("Done Downloading!\n");
	}
	else if(Flags[NETWORK_SCAN] == 0x01)
	{
		//printf("Network Scan!!\n"); 
		//sendAvailableNetworks(); 
	}
	else if(Flags[NETWORK_CONNECT] == 0x01)
	{
		//printf("Network Connect!\n");
		//networkConnect("Wi-Pro", "brightshoe902"); 
		//getFileEthernet()
	}
	//_delay_ms(500);
	clearFlags(); 
	TIMSK1 |= (1 << TOIE1);
}



//Timer overflow vector for polling 
ISR(TIMER1_OVF_vect)
{
	cli(); 
	printf("Flag Status\n");
	getFlagStatus(); 
	TCNT1L = 0x00;
	TCNT1H = 0x00;
	sei();  
}
