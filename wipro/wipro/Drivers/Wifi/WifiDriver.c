/*
 * WifiDriver.c
 *
 * Created: 3/7/2015 5:10:03 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../Print/PrintDriver.h"
#include "WifiDriver.h"
#include "../RAM/RAMDriver.h"

volatile unsigned char headerBuffer[endHeader];
char fullCommand[100]; 
volatile unsigned int bufferStart; 
volatile int testPrint; 
volatile char buff; 
//volatile uint16_t transLength = 0;

volatile uint16_t i;
volatile uint8_t receiveWifiFlag; 
volatile uint16_t transLength;
volatile uint32_t RAMAddress;

volatile uint8_t endHeaderFlag; 
volatile uint8_t compressFlag; 
volatile uint8_t secondNibble; 
volatile uint8_t multiReceiveFlag; 
uint8_t compressBuff; 

void wifiDriverInit()
{
	bufferStart = 0; 
	testPrint = 0; 
	buff = 0; 
	i = 0; 
	receiveWifiFlag = 0; 
	transLength = 0; 
	RAMAddress = 0; 
	memset(headerBuffer, 0x00, endHeader);
	endHeaderFlag = 0;  
	compressFlag = 0; 
	secondNibble = 0; 
}

void setTestPrint(int print)
{
	testPrint = print; 
}

void setCompressFlag(uint8_t compress)
{
	PORTD &= ~(1<<CTS); 
	compressFlag = compress;
	PORTD |= (1<<CTS); 
}

void setReceiveCounter(int val)
{
	i = val; 
}

void setMultiReceiveFlag(uint8_t receiveFlag)
{
	multiReceiveFlag = receiveFlag; 
}

void uart_init()
{
	UBRR1H = (((F_CPU/BAUD_RATE)/16)-1)>>8;	// set baud rate
	UBRR1L = (((F_CPU/BAUD_RATE)/16)-1);
	UCSR1B = (1<<RXEN1)|(1<<TXEN1); // enable Rx & Tx
	UCSR1C =  (1<<UCSZ11)|(1<<UCSZ10); // config USART; 8N1
	UCSR1B |= (1<<RXCIE1);	//Enable Receive Interrupt 
	DDRA |= (1<<PORTA1);
}

uint16_t getStringLen(unsigned char* p)
{
	uint16_t count = 0; 
	while(*p != '\0')
	{
		count++; 
		p++; 
	}
	return count; 
}

int uart_send(char* data, unsigned int length)
{
	//printf("Uart Send Function: %s\nAddress: %p", data, data);
	uint8_t i = 0;
	//UCSR1B |= (1<<RXCIE1);
	receiveWifiFlag = 0;
	memset(headerBuffer, 0x00, endHeader);
	PORTD |= (1<<RTS);
	//while(!(PIND & (1<<CTS))){
		//_delay_us(100);
		//printf("Waiting..\n");
	//} 
	while(i < length){
		while(!(UCSR1A & (1<<UDRE1)));
		UDR1 = data[i];
		i++;
	}
	//while(!(PIND & (1<<CTS))){_delay_us(100);}
	printf("Sent!\n");
	return 0;
}

unsigned char uart_receive(unsigned char* data, unsigned char size)
{
	uint16_t i = 0;
 
	while(1 < size - 1) {
		unsigned char c;
		while (!(UCSR1A & (1<<RXC1)));
		c = UDR1;
		//printf("Moving Received Byte %d: %c\n", i, c);
		//_delay_ms(500);
		printf("%c",c);
		if(c == '\0') break; 
		data[i] = c; 
		i++; 
	} 
	//Null terminating character 
	data[i] = 0;
	printf("%s", data);
	return i + 1;  
}

unsigned char uart_receiveChar()
{
	//printf("Receiving...\n");
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1; 
}

int enableReceiveINT()
{
	UCSR1B |= (1<<RXCIE1);
	sei(); 
	return 1; 
}

int disableReceiveINT()
{
	UCSR1B &= ~(1<<RXCIE1);
	return 1; 
}

char* getReceiveBuffer()
{
	//printf("Waiting for Receive to Complete...\n");
	//Wait for receiving to be completed
	//sendFlag accounts for the delay between sending and receiving 
	while(!receiveWifiFlag & 1)
	{
		//While loop does not work correctly without a delay 
		//An issue with the compiler or the stack pointer when invoking the interrupt 
		_delay_us(100);
		//printf("Receive Flag: %d\n", receiveWifiFlag);
	}
	//_delay_ms(500);
	//begin receiving
	//printf("Received Data: %s\n", receiveBuffer);
	return ""; 
}

int receiveStatus()
{
	while(receiveWifiFlag == 0)
	{
		//While loop does not work correctly without a delay
		//An issue with the compiler or the stack pointer when invoking the interrupt
		_delay_us(100);
		//printf("Receive Flag: %d\n", receiveWifiFlag);
		//printf("Loooop\n");
	}
	
	return 1; 
}

unsigned int buildTransmissionLength()
{
	if(testPrint)
	{
		//printf("Header: ");
		for(int i = 0; i < endHeader; i++)
		{
			printf("Value: 0x%02x, Address: %p\n", headerBuffer[i], headerBuffer + i);
			//printf("0x%02x ", receiveBuffer[i]);
		}
		printf("\n");
	}
	transLength = 0; 
	transLength += (headerBuffer[ones] & 0x0F);
	transLength += (headerBuffer[tens] & 0x0F) * 10; 
	transLength += (headerBuffer[hundreds] & 0x0F) * 100; 
	transLength += (headerBuffer[thousands] & 0x0F) * 1000; 
	transLength += (headerBuffer[tenThousands] & 0x0F) * 10000; 
	//if(transLength < MaxRecSize)
	return transLength; 
	//else
		////8 is the length of the header 
		//return MaxRecSize - 8; 
}

uint16_t getTransmissionLength()
{
	return transLength; 
}

char* getMessageHeader()
{
	return headerBuffer; 
}

int errorCheck() 
{
	while(!receiveWifiFlag & 1)
	{
		//While loop does not work correctly without a delay
		//An issue with the compiler or the stack pointer when invoking the interrupt
		_delay_us(100);
	}
	
	char* header = getMessageHeader();
	//0 denotes a successful command 
	if(header[errorCode] != '0')
		return 1; 
	else 
		return 0; 
}

void updateRAMAddress(uint32_t address)
{
	RAMAddress = address; 
}

unsigned int sendCommand(int8_t prefix, char* command, char* value)
{
	//char* fullCommand = (char *)malloc(MaxSendSize);
	switch(prefix)
	{
		case NOPREFIX: 
			break; 
		case GET:
			strcpy(fullCommand, "get "); 
			//strcat(fullCommand, "get ");
			break; 
		case SET: 
			strcat(fullCommand, "set ");
			break; 
		default:
			return 0; 
			break; 
	}
	//printf("Command: %s Length: %d, Address: %p\n", fullCommand, strlen(fullCommand), fullCommand);
	//printf("Command: %s Length: %d, Address: %p\n", command, strlen(command),  command);
	strcat(fullCommand, command);
	//printf("Full Command: %s", fullCommand);	
	if(value != NOVAL)
	{
		strcat(fullCommand, " ");
		strcat(fullCommand, value);
	}
	
	//printf("fullCommand: %s\n", fullCommand);
	strcat(fullCommand, ENDCOMMAND);
	uint16_t length = strlen(fullCommand);
	printf("Command: %s Length: %d, Address: %p\n", fullCommand, length, fullCommand);
	uart_send(fullCommand, length);
	memset(fullCommand, 0x00, 100);
	//PORTD |= (1<<RTS);
	printf("Freed!\n");
	printf("Returning!\n");
	return 1; 
}

//ISR(USART1_RX_vect)
//{
	//cli();
	////if(testPrint)
	////printf("Interrupt!\n");
	//if(!bufferStart)
	//{
		//buff = uart_receiveChar();
		////Header always begins with letter 'R'
		//if(buff == headerStartVal)
		//{
			//bufferStart = 1;
			//
			//if(testPrint)
				//printf("Beginning Found @ %d\n", i);
			//if(testPrint)
				//printf("Header: %c @ address %p", headerBuffer[i], headerBuffer);
		//}
	//}
	//
	//else if(!endHeaderFlag)
	//{
		//buff = uart_receiveChar(); 
		//
		//if(buff == 0x00)
		//{
			//endHeaderFlag = 1; 
		//}
	//}
	//
	//else
	//{
		//buff = uart_receiveChar(); 
		//if(buff != 0x00)
		//{
			////printf("Translength: %d", transLength);
			//PORTD &= ~(1<<CTS);
			//RAMWriteByte(buff, RAMAddress + i - endHeader -1);
			////printf("Received String: %c @ location %d\n", receiveBuffer[i], i);
			////i++;
		//}
//
		//else
		//{
			////printf("End of String!\n");
			//RAMWriteByte(0x00, RAMAddress + i - endHeader -1);
			////cli();
			//i = 0;
			//bufferStart = 0;
			//endHeaderFlag = 0; 
			////done receiving
			//receiveWifiFlag = 1;
			//printf("Transmission Length: %d\n", i);
			//printf("Done Receiving!\n");
		//}
	//}
	////if(bufferStart && !receiveWifiFlag)
	//i++;
	//
	//PORTD |= (1<<CTS);
	//sei();
//}

ISR(USART1_RX_vect)
{
	cli();
	//if(testPrint)
		//printf("Interrupt!\n");
	if(!bufferStart)
	{
		buff = uart_receiveChar();
		//Header always begins with letter 'R'
		if(buff == headerStartVal)
		{
			PORTD &= ~(1<<RTS);
			//if(testPrint)
				//printf("Beginning Found @ %d\n", i); 
				
			i = 0;
			headerBuffer[i] = buff; 
			
				//if(testPrint)
					//printf("Header: %c @ address %p", headerBuffer[i], headerBuffer); 
					
			bufferStart = 1;
			PORTD |= (1<<RTS); 
		}
	}
	
	else
	{
		//printf("Beginning of buffer.\n");
		//Grab Receive Header
		if(i < endHeader)
		{
			PORTD &= ~(1<<RTS);
			headerBuffer[i] = uart_receiveChar();
			//if(testPrint)
				//printf("Header: %c @ address %p\n", headerBuffer[i], headerBuffer + i);
			//i++; 
			//RAMWriteByte(uart_receiveChar(), i);
			PORTD |= (1<<RTS); 
		}
		else if(i == endHeader)
		{
			transLength = buildTransmissionLength();
			//i++; 
			if(testPrint)
				printf("Transmission Length: %d\n", transLength);
		}
		else
		{
			if(i < transLength + endHeader)
			{
				//printf("Translength: %d", transLength);
				buff = uart_receiveChar();
				if(compressFlag == 1)
				{
					//printf("Compressing!\n");
					PORTD &= ~(1<<CTS);
					if(buff == ':')
					{
						PORTD &= ~(1<<RTS);
						RAMWriteByte(buff, RAMAddress + i - endHeader -1);
					}
					else if(!secondNibble)
					{
						//Mask the ASCII Nibble 
						compressBuff = (buff & 0x0F);
						//Shift it into the upper nibble  
						compressBuff <<= 4; 
						secondNibble =  1;
					}
					else
					{
						compressBuff |= buff; 
						RAMWriteByte(compressBuff, RAMAddress + i - endHeader -1);
						secondNibble = 0; 
					}
				}
				else
				{
					RAMWriteByte(buff, RAMAddress + i - endHeader -1);	
				}
				
				//printf("Received String: %c @ location %d\n", receiveBuffer[i], i);
				//i++; 
			}

			else
			{
				//printf("End of String!\n");
				RAMWriteByte(0x00, RAMAddress + i - endHeader -1);
				//cli();
				i = 0;
				bufferStart = 0;
				compressFlag = 0; 
				secondNibble = 0; 
				compressBuff = 0x00; 
				//done receiving
				receiveWifiFlag = 1;
				printf("Transmission Length: %d\n", buildTransmissionLength());
				printf("Done Receiving!\n");
			}
		}
		//printf("%d\n", i); 
		//i++; 
	}
	//if(bufferStart && !receiveWifiFlag)
	i++; 
		
	PORTD |= (1<<CTS); 
	sei(); 
}
