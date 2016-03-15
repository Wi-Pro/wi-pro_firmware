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
#include "utl_pub_print.h"

void uart_flush()
{
	unsigned char dummy;
	while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

int uart_putch(char ch,FILE *stream)
{
	if (ch == '\n')
	uart_putch('\r', stream);
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0=ch;
	return 0;
}

int uart_getch(FILE *stream)
{
	unsigned char ch;
	while (!(UCSR0A & (1<<RXC0)));
	ch=UDR0;

	/* Echo the Output Back to terminal */
	uart_putch(ch,stream);

	return ch;
}

void ansi_cl(void)
{
	// ANSI clear screen: cl=\E[H\E[J
	putchar(27);
	putchar('[');
	putchar('H');
	putchar(27);
	putchar('[');
	putchar('J');
}

void ansi_me(void)
{
	// ANSI turn off all attribute: me=\E[0m
	putchar(27);
	putchar('[');
	putchar('0');
	putchar('m');
}


int uart_sendStream(char ch,FILE *stream)
{
	if (ch == '\n')
	uart_sendStream('\r', stream);
	while (!(UCSR2A & (1<<UDRE2)));
	UDR2=ch;
	return 0;
}

int uart_receiveStream(FILE *stream)
{
	unsigned char ch;
	while (!(UCSR2A & (1<<RXC2)));
	ch=UDR2;

	/* Echo the Output Back to terminal */
	uart_sendStream(ch,stream);

	return ch;
}

FILE uart_stream = FDEV_SETUP_STREAM(uart_sendStream, uart_receiveStream, _FDEV_SETUP_RW);

void uart2_init()
{
	UBRR2H = (((F_CPU/BAUD_RATE)/16)-1)>>8;	// set baud rate
	UBRR2L = (((F_CPU/BAUD_RATE)/16)-1);
	UCSR2B = (1<<RXEN2)|(1<<TXEN2); 		// enable Rx & Tx
	UCSR2C=  (1<<UCSZ21)|(1<<UCSZ20);  	       // config USART; 8N1
	//DDRD |= (1<<PORTD1)|(1<<PORTD0);
	// Define Output/Input Stream
	stdout = stdin = &uart_stream;
}

void UARTStream_Init()
{
	//DDRA = 0xFF;
	// Set the PORTD as Output:
	//DDRD=0xFF;
	//PORTD=0x00;

	// Define Output/Input Stream
	stdout = stdin = &uart_stream;
	// Initial UART Peripheral
	uart_init();
	 //Clear Screen
	 ansi_me();
	 ansi_cl();
	 ansi_me();
	 ansi_cl();
	 uart_flush();
}

