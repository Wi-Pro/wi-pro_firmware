/*
 * PrintDriver.h
 *
 * Created: 2/15/2015 2:56:11 PM
 *  Author: Brandon
 */ 
#include <stdio.h>

#ifndef PRINTDRIVER_H_
#define PRINTDRIVER_H_

#define BAUD_RATE 2400 
#define F_CPU 1000000UL

void uart_init(void);
void uart_flush(void);
int uart_send(char ch,FILE *stream);
int uart_receive(FILE *stream);



#endif /* PRINTDRIVER_H_ */