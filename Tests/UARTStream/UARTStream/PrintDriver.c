/*
 * PrintDriver.c
 *
 * Created: 2/15/2015 2:44:02 PM
 *  Author: Brandon
 */ 
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PrintDriver.h"

void uart_flush(void)
{
	unsigned char dummy;
	while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

int uart_send(char ch,FILE *stream)
{
	if (ch == '\n')
	uart_send('\r', stream);
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0=ch;
	return 0;
}

int uart_receive(FILE *stream)
{
	unsigned char ch;
	while (!(UCSR0A & (1<<RXC0)));
	ch=UDR0;

	/* Echo the Output Back to terminal */
	uart_send(ch,stream);

	return ch;
}

FILE uart_stream = FDEV_SETUP_STREAM(uart_send, uart_receive, _FDEV_SETUP_RW);

void uart_init(void)
{
	UBRR0H = (((F_CPU/BAUD_RATE)/16)-1)>>8;	// set baud rate
	UBRR0L = (((F_CPU/BAUD_RATE)/16)-1);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0); 		// enable Rx & Tx
	UCSR0C=  (1<<UCSZ01)|(1<<UCSZ00);  	       // config USART; 8N1
	// Define Output/Input Stream
	stdout = stdin = &uart_stream;
}