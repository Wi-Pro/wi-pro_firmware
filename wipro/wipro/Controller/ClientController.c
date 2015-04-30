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
#include "../Controller/ClientController.h"
#include "../Drivers/Ethernet/Ethernet.h"
#include "../Drivers/Ethernet/EthernetDriver.h"
#include "../Drivers/Print/PrintDriver.h"
#include "../Drivers/RAM/RAMDriver.h"
#include "../Drivers/Wifi/Wifi.h"
#include "../Drivers/Wifi/WifiDriver.h"
#include "../Drivers/RAM/MemoryMap.h"
#include "../Program/Program.h"
#include "../Program/AVR.h"
#include "../Program/ProgramDriver.h"

uint8_t Flags[FLAG_ARRAY_LENGTH]; 
char filepath[100]; 
uint32_t signatureBytes; 

void connectionInit()
{
	uart2_init();
	uart_init(); 
	SPI_Init();
	flagInit(); 
	ethernetPlugInit(); 
	memset(filepath, 0x00, 100);
	Flags[WIFI] = 1; 
	//checkWifiStatus(); 
}

void flagInit()
{
	//Clear all flags 
	for(int i = 0; i < FLAG_ARRAY_LENGTH; i++)
	{
		Flags[i] = 0; 
	}
	
	if(!(PINE & (1<<INT5)))
	{
		printf("Ethernet Enabled!\n"); 
		ethernetInit();
		Flags[ETHERNET] = 1; 
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

void ethernetPlugInit()
{
	EIMSK = (1<<INT5);
	//Double Edge Triggered 
	EICRB = (1<<ISC50);
}

//Checks to see if Ethernet or Wi-Fi is available 
int checkWifiStatus()
{
	Flags[WIFI] = networkTest(); 
	return 1; 
}

int clearFlags()
{
	if(Flags[ETHERNET])
	{
		memset(filepath, 0x00, 100);
		strcpy(filepath, "GET ");
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
	//Flags[WIFI] = 1;
	if(Flags[WIFI])
	{
		uint16_t transLength; 
		strcpy(filepath, URL);
		strcat(filepath, HEX_FILE);
		//setTestPrint(1);
		//printf("Compress Flag Set!");
		//setCompressFlag(1);
		transLength = getFileWifi(filepath, 1, HEX_FILE_ADDRESS, 1);
		printf("WTF HELLO4!!!\n");
		printf("Uncompressed Trans Length: %d\n", transLength); 
		//RAMPrint(HEX_FILE_ADDRESS, 100);
		compressFile(transLength); 
		memset(filepath, 0x00, 100);
		strcpy(filepath, URL);
		strcat(filepath, DEVICE_FILE);
		getFileWifi(filepath, 1, STATUS_FLAG_ADDRESS + DEVICE_ID, 1); 
		//writeHexFileTest(); 
		printf("Compressed!\n"); 
		//setCompressFlag(0); 
		//RAMPrint(HEX_FILE_ADDRESS, 500);
		//Program(ATtiny2313); 
		//printf("Hex File Downloaded!\n");
	}
	
	return 1; 
}

int getFlagStatus()
{
	TIMSK1 &= ~(1 << TOIE1);
	//Flags[ETHERNET] = 0; 
	//Flags[WIFI] = 0; 
	//Normally Flags[ETHERNET] == 1 
	if(0){
		getFileEthernet(FLAG_FILE, 1, STATUS_FLAG_ADDRESS, 3); 
		//printf("Ram Print: ");
		//RAMPrint(STATUS_FLAG_ADDRESS-10, 20);
		//printf("\n");
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
		//printf("Ram Print: ");
		//RAMPrint(STATUS_FLAG_ADDRESS, 3);
		//printf("\n");
		Flags[PROGRAM] = (RAMReadByte(STATUS_FLAG_ADDRESS) & 0x0F);
		Flags[NETWORK_SCAN] = (RAMReadByte(STATUS_FLAG_ADDRESS + 1) & 0x0F);
		Flags[NETWORK_CONNECT] = (RAMReadByte(STATUS_FLAG_ADDRESS + 2) & 0x0F);
		
	}
	memset(filepath, 0x00, 100); 
	//Perform actions based on flags 
	if(Flags[PROGRAM] == 0x01)
	{
		//Program Function 
		printf("Program!\n");
		//PORTD &= ~(1<<CTS);
		//wifiDriverInit(); 
		//PORTD |= (1<<CTS); 
		//_delay_ms(2000);
		LED_PORT |= (1<<LED_Yellow);
		do 
		{
			getHexFile();
		} while (!checkSum());
		
		 
		Flags[DEVICE_ID] = (RAMReadByte(STATUS_FLAG_ADDRESS + DEVICE_ID) & 0x0F);
		//printf("Device ID: %d\n", Flags[DEVICE_ID]); 
		//printf("Signature Byte: 0x%08X\n", signatureBytes); 
		switch(Flags[DEVICE_ID])
		{
			case ATtiny2313_ID:
				Program(ATtiny2313);
				break;
			case ATmega324PA_ID: 
				Program(ATmega324PA);
				break;
			default:
				printf("Error, bad device ID!\n");
				break; 
		}
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
	return 1; 
}

//Timer overflow vector for polling 
ISR(TIMER1_OVF_vect)
{
	//cli(); 
	TIMSK1 &= ~(1 << TOIE1);
	printf("Flag Status\n");
	getFlagStatus(); 
	TCNT1L = 0x00;
	TCNT1H = 0x00;
	TIMSK1 |= (1 << TOIE1);
	//sei();  
}

ISR(INT5_vect)
{
	cli();
	if(PINE & (1<<INT5))
	{
		Flags[ETHERNET] = 0; 
		//checkWifiStatus(); 
		printf("Rising Edge!\n");
	}
	else
	{
		Flags[ETHERNET] = 1; 
		ethernetInit();
		printf("Falling Edge!\n");
	}
	sei();
}
