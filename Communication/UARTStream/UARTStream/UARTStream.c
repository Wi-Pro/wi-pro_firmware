/*
 * UARTStream.c
 *
 * Created: 2/15/2015 2:43:15 PM
 *  Author: Brandon
 */ 


#include <avr/io.h>
#include <string.h>
#include "PrintDriver.h"

int main(void)
{
	uart_init(); 
	printf("This is a test\n\n");
	unsigned int testNum = 30; 
	printf("%d", testNum); 
    while(1)
    {
        //TODO:: Please write your application code 
    }
}