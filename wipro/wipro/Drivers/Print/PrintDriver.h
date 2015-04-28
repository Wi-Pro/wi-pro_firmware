/*
 * PrintDriver.h
 *
 * Created: 2/15/2015 2:56:11 PM
 *  Author: Brandon
 */ 
#include <stdio.h>

#ifndef PRINTDRIVER_H_
#define PRINTDRIVER_H_

#define BAUD_RATE 62500 
#define F_CPU 8000000UL

#define CTS PORTD4
#define RTS PORTD5 

void uart2_init();
void uart1_init();
void uart_flush();
//int uart_send(unsigned char data[]);
int uart_sendStream(char ch,FILE *stream);
int uart_receiveStream(FILE *stream);



#endif /* PRINTDRIVER_H_ */