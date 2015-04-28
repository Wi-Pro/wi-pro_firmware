/*
 * WifiDriver.h
 *
 * Created: 3/7/2015 5:10:18 PM
 *  Author: Brandon
 */ 


#ifndef WIFIDRIVER_H_
#define WIFIDRIVER_H_

#define BAUD_RATE 62500
#define F_CPU 8000000UL

#define MaxRecSize 25
#define MaxSendSize 50

//Wi-Fi Commands
#define NOPREFIX -1
#define GET 1 
#define SET 2
#define NOVAL ""
#define ENDCOMMAND "\r\n\0"

//Wi-Fi Receive Header File 
#define response 0
#define errorCode 1
#define tenThousands 2
#define thousands 3
#define hundreds 4
#define tens 5
#define ones 6
#define endHeader 9

#define headerStartVal 'R'

//I/Os 
//#define RTS PORTD4
//#define CTS PORTD5 

void wifiDriverInit();
void setTestPrint(int print);
void setReceiveCounter(int val);
void setCompressFlag(uint8_t compress);
void uart_init();
int uart_send(char* data, unsigned int length);
int uart_sendChar(unsigned char data);
int enableReceiveINT();
int disableReceiveINT();
unsigned char uart_receive(unsigned char *data, unsigned char size);
unsigned char uart_receiveChar();
char* getReceiveBuffer();
char* getMessageHeader();
unsigned int sendCommand(int8_t prefix, char* command, char* value);
uint16_t getStringLen(unsigned char* p);
int waitForReceive();
int errorCheck();
unsigned int buildTransmissionLength();
uint16_t getTransmissionLength();
void updateRAMAddress(uint32_t address);
int receiveStatus();

#endif /* WIFIDRIVER_H_ */