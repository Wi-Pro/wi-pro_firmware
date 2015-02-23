/*
 * EthernetModule.c
 *
 * Created: 2/7/2015 6:11:09 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "EthernetLibrary.h"

unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
unsigned char ip_addr[] = {192,168,1,5};
unsigned char sub_mask[] = {255,255,255,0};
unsigned char gtw_addr[] = {192,168,1,1};
	
unsigned char server_ip_addr[] = {104,131,36,80};
//unsigned char server_ip_addr[] = {104,104,36,81};
unsigned char server_port[] = {00,80}; 
unsigned char source_port[] = {00,80};
	
unsigned int S0_RX_BASE;
unsigned int S0_RX_MASK;
unsigned int S0_TX_BASE;
unsigned int S0_TX_MASK;

void uart_init(void)
{
	UBRR0H = (((F_CPU/BAUD_RATE)/16)-1)>>8;	// set baud rate
	UBRR0L = (((F_CPU/BAUD_RATE)/16)-1);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0); 		// enable Rx & Tx
	UCSR0C =  (1<<UCSZ01)|(1<<UCSZ00);  	// config USART; 8N1
}

void uart_flush(void)
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

void SPI_Write(unsigned int addr,unsigned char data)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<SPI_CS);
	// Start Wiznet W5100 Write OpCode transmission
	SPDR0 = WIZNET_WRITE_OPCODE;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR0 = (addr & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = addr & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// Start Data transmission
	SPDR0 = data;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
}

unsigned char SPI_Read(unsigned int addr)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<SPI_CS);
	// Start Wiznet W5100 Read OpCode transmission
	SPDR0 = WIZNET_READ_OPCODE;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address High Bytes transmission
	SPDR0 = (addr & 0xFF00) >> 8;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));
	// Start Wiznet W5100 Address Low Bytes transmission
	SPDR0 = addr & 0x00FF;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// Send Dummy transmission for reading the data
	SPDR0 = 0x00;
	// Wait for transmission complete
	while(!(SPSR0 & (1<<SPIF0)));

	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
	return(SPDR0);
}

void W5100_Init(void)
{
	// Ethernet Setup
	printf("Enterting Ethernet Setup\n");
	// Setting the Wiznet W5100 Mode Register: 0x0000
	SPI_Write(MR,0x80);            // MR = 0b10000000;
	_delay_ms(1);
	printf("Reading MR: %d\n\n",SPI_Read(MR));
	// Setting the Wiznet W5100 Gateway Address (GAR): 0x0001 to 0x0004
	printf("Setting Gateway Address %d.%d.%d.%d\n",gtw_addr[0],gtw_addr[1],\
	gtw_addr[2],gtw_addr[3]);
	SPI_Write(GAR + 0,gtw_addr[0]);
	SPI_Write(GAR + 1,gtw_addr[1]);
	SPI_Write(GAR + 2,gtw_addr[2]);
	SPI_Write(GAR + 3,gtw_addr[3]);
	_delay_ms(1);
	printf("Reading GAR: %d.%d.%d.%d\n\n",SPI_Read(GAR + 0),SPI_Read(GAR + 1),\
	SPI_Read(GAR + 2),SPI_Read(GAR + 3));
	// Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
	printf("Setting Source Address %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",mac_addr[0],mac_addr[1],\
	mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	SPI_Write(SAR + 0,mac_addr[0]);
	SPI_Write(SAR + 1,mac_addr[1]);
	SPI_Write(SAR + 2,mac_addr[2]);
	SPI_Write(SAR + 3,mac_addr[3]);
	SPI_Write(SAR + 4,mac_addr[4]);
	SPI_Write(SAR + 5,mac_addr[5]);
	_delay_ms(1);
	printf("Reading SAR: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n\n",SPI_Read(SAR + 0),SPI_Read(SAR + 1),\
	SPI_Read(SAR + 2),SPI_Read(SAR + 3),SPI_Read(SAR + 4),SPI_Read(SAR + 5));
	// Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
	printf("Setting Sub Mask Address %d.%d.%d.%d\n",sub_mask[0],sub_mask[1],\
	sub_mask[2],sub_mask[3]);
	SPI_Write(SUBR + 0,sub_mask[0]);
	SPI_Write(SUBR + 1,sub_mask[1]);
	SPI_Write(SUBR + 2,sub_mask[2]);
	SPI_Write(SUBR + 3,sub_mask[3]);
	_delay_ms(1);
	printf("Reading SUBR: %d.%d.%d.%d\n\n",SPI_Read(SUBR + 0),SPI_Read(SUBR + 1),\
	SPI_Read(SUBR + 2),SPI_Read(SUBR + 3));
	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	printf("Setting IP Address %d.%d.%d.%d\n",ip_addr[0],ip_addr[1],\
	ip_addr[2],ip_addr[3]);
	SPI_Write(SIPR + 0,ip_addr[0]);
	SPI_Write(SIPR + 1,ip_addr[1]);
	SPI_Write(SIPR + 2,ip_addr[2]);
	SPI_Write(SIPR + 3,ip_addr[3]);
	_delay_ms(1);
	printf("Reading SIPR: %d.%d.%d.%d\n\n",SPI_Read(SIPR + 0),SPI_Read(SIPR + 1),\
	SPI_Read(SIPR + 2),SPI_Read(SIPR + 3));

	// Setting the Wiznet W5100 RX and TX Memory Size, we use 2KB for Rx/Tx 4 channels
	//printf("Setting Wiznet RMSR and TMSR\n\n");
	//SPI_Write(RMSR,0x55);
	//SPI_Write(TMSR,0x55);

	printf("Done Wiznet W5100 Initialized!\n");
	//enableEthernetInterrupt(); 
	Memory_Init(); 
	Server_Connect();
	char message[] = ""; 
	char receiveMessage[100];
	char* messagePointer = message; 
	char* recMessagePointer = receiveMessage;
	messagePointer = "GET /index.html HTTP/1.1";
	SendData(0, messagePointer, strlen((char *)messagePointer)); 
	ReceiveData(0, recMessagePointer, 100);
	printf("Receive Size: %d\n", ReceiveSize());
	printf("Message: %s", receiveMessage);
}

int Server_Connect()
{
	do 
	{
		printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		SPI_Write(S0_CR, CLOSE);
		SPI_Write(S0_MR, TCP_MODE);
		printf("Setting Source Port on Socket 0 %d%d\n",source_port[0],source_port[1]);
		SPI_Write(S0_SPORT + 0, source_port[0]);
		SPI_Write(S0_SPORT + 1, source_port[1]);
		printf("Reading Source Port on Socket 0 %d%d\n\n",SPI_Read(S0_SPORT),SPI_Read(S0_SPORT + 1));
		
		printf("Setting Socket 0 to Open\n\n");
		SPI_Write(S0_CR, OPEN);
		printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		
	} while (SPI_Read(S0_SR) != SOCK_INIT);
	
	//Write Server IP to Socket 0 IP Register
	printf("Setting Server IP Address %d.%d.%d.%d\n",server_ip_addr[0],server_ip_addr[1],\
	server_ip_addr[2],server_ip_addr[3]); 
	SPI_Write(S0_DIPR + 0,server_ip_addr[0]);
	SPI_Write(S0_DIPR + 1, server_ip_addr[1]);
	SPI_Write(S0_DIPR + 2,server_ip_addr[2]);
	SPI_Write(S0_DIPR + 3, server_ip_addr[3]);
	printf("Reading S0_DIPR: %d.%d.%d.%d\n\n",SPI_Read(S0_DIPR + 0),SPI_Read(S0_DIPR + 1),\
	SPI_Read(S0_DIPR + 2),SPI_Read(S0_DIPR + 3));
	printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
	
	//Write Server Port to Socket 0 IP Register 
	printf("Setting Server Port\n");
	SPI_Write(S0_DPORT + 0, server_port[0]); 
	SPI_Write(S0_DPORT + 1, server_port[1]);
	printf("Reading S0_DPORT: %d%d\n\n",SPI_Read(S0_DPORT + 0),SPI_Read(S0_DPORT + 1)); 
	printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
	
	//Try to connect 
	SPI_Write(S0_CR, CONNECT);
	while((SPI_Read(S0_IR) & 0x1F) != 0); 
	//Check to see if connection has been established 
	unsigned int delayCount = 0; 
	while(SPI_Read(S0_SR) == 0x15)
	{
		//printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		_delay_ms(3);
		delayCount += 1; 
		//Every 3 seconds
		if(delayCount == 1000)
		{
			printf("Trying to Establish Connection...\n");
			delayCount = 0; 
		}
	}
	
	printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
	if(SPI_Read(S0_SR) == SOCK_ESTABLISHED)
	{
		printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		printf("Connection Succeeded!\n\n");
		return 1; 
	}
	//Connection Failed 
	else
	{
		printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		printf("Could not connect. Exiting...\n\n");
		return 0; 
	}
	
}

void Memory_Init()
{
	//assign 8kb rx memory to socket 0 
	SPI_Write(RMSR, 0x55);
	//assign 8kb tx memory to socket 0 
	SPI_Write(TMSR, 0x55);	
	
	S0_TX_MASK = 0x7FF;
	S0_TX_BASE = 0x4000;
	S0_RX_MASK = 0x07FF;
	S0_RX_BASE = 0x6000;
	
	//8k RX and TX Socket 0 
	//S0_TX_MASK = 0x1FFF; 
	//S0_TX_BASE = 0x4000; 
	//S0_RX_MASK = 0x1FFF; 
	//S0_RX_BASE = 0x6000; 
}

//void SendData(uint8_t sock,const uint8_t *buf,uint16_t buflen)
//{
	//printf("Initializing Memory...\n");
	//Memory_Init(); 
	//printf("Sending Data...\n");
	//unsigned int freeSize = 0; 
	//printf("FreeSize: %d\n", freeSize);
	//freeSize = SPI_Read(S0_TX_FSR);
	//printf("FreeSize: %d\n", freeSize);
	//freeSize <<= 8; 
	//printf("FreeSize: %d\n", freeSize);
	//freeSize |= SPI_Read(S0_TX_FSR + 1);
	//printf("FreeSize: %d\n", freeSize);
	//printf("Free Size to Transmit: %d\n", freeSize);
	////Calculate the offset address
	//unsigned int offset = SPI_Read(S0_TX_WR); 
	//offset <<= 8; 
	//offset |= SPI_Read(S0_TX_WR + 1); 
	//offset &= S0_TX_MASK; 
	//printf("Address Offset: %d\n", offset);
	//
	////Calculate start physical address 
	//unsigned int startAddress = S0_TX_BASE + offset; 
	//printf("Starting Physical Address: %d", startAddress);
//}

int SendData(uint8_t sock,const uint8_t *buffer,uint16_t bufferLength)
{
	uint16_t ptr,offaddr,realaddr,txsize,timeout;

	if (bufferLength <= 0 || sock != 0) return 0;
	//#if _DEBUG_MODE
	printf("Send Size: %d\n",bufferLength);
	//#endif
	// Make sure the TX Free Size Register is available
	txsize = SPI_Read(S0_TX_FSR);
	txsize = (((txsize & 0x00FF) << 8 ) + SPI_Read(S0_TX_FSR + 1));
	//#if _DEBUG_MODE
	printf("TX Free Size: %d\n",txsize);
	//#endif
	timeout = 0;
	//Writing our buffer to the transmit buffer
	while (txsize < bufferLength) {
		_delay_ms(1);
		txsize = SPI_Read(S0_TX_FSR);
		txsize = (((txsize & 0x00FF) << 8 ) + SPI_Read(S0_TX_FSR + 1));
		printf("TX Size: %d", txsize);
		// Timeout for approx 1000 ms
		if (timeout++ > 1000) {
			//#if _DEBUG_MODE
			printf("TX Free Size Error!\n");
			//#endif
			// Disconnect the connection
			socketCommand(DISCON);
			while(SPI_Read(S0_CR));
			return 0;
		}
	}

	// Read the Tx Write Pointer
	ptr = SPI_Read(S0_TX_WR);
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_TX_WR + 1));
	//#if _DEBUG_MODE
	printf("TX Buffer: %x\n",offaddr);
	//#endif

	while(bufferLength) {
		bufferLength--;
		// Calculate the real W5100 physical Tx Buffer Address
		realaddr = S0_TX_BASE + (offaddr & S0_TX_MASK);
		//printf("TX Real Address: %d\n", realaddr);
		// Copy the application data to the W5100 Tx Buffer
		SPI_Write(realaddr,*buffer);
		offaddr++;
		buffer++;
	}

	// Increase the S0_TX_WR value, so it point to the next transmit
	SPI_Write(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
	SPI_Write(S0_TX_WR + 1,(offaddr & 0x00FF));

	// Now Send the SEND command
	socketCommand(SEND);

	// Wait for Sending Process
	while(SPI_Read(S0_CR));

	return 1;
}

int ReceiveData(uint8_t sock,uint8_t *buffer,uint16_t bufferLength)
{
	uint16_t ptr,offaddr,realaddr;

	if (bufferLength <= 0 || sock != 0) return 1;

	// If the request size > MAX_BUF,just truncate it
	if (bufferLength > MAX_BUFF)
	bufferLength = MAX_BUFF - 2;
	// Read the Rx Read Pointer
	ptr = SPI_Read(S0_RX_RD);
	printf("RX Read Pointer: %d\n", ptr);
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_RX_RD + 1));
	//#if _DEBUG_MODE
	printf("RX Buffer: %x\n",offaddr);
	//#endif

	while(bufferLength) {
		bufferLength--;
		realaddr = S0_RX_BASE + (offaddr & S0_RX_MASK);
		//printf("Real Address: %d\n", realaddr);
		*buffer = SPI_Read(realaddr);
		offaddr++;
		buffer++;
	}
	*buffer='\0';        // String terminated character

	// Increase the S0_RX_RD value, so it point to the next receive
	SPI_Write(S0_RX_RD,(offaddr & 0xFF00) >> 8 );
	SPI_Write(S0_RX_RD + 1,(offaddr & 0x00FF));

	// Now Send the RECV command
	//SPI_Write(S0_CR,CR_RECV);
	socketCommand(RECV);
	_delay_us(5);    // Wait for Receive Process

	return 1;
}

uint16_t ReceiveSize(void)
{
	return ((SPI_Read(S0_RX_RSR) & 0x00FF) << 8 ) + SPI_Read(S0_RX_RSR + 1);
}

void socketCommand(uint8_t command) 
{
	SPI_Write(S0_CR, command);
}

void enableEthernetInterrupt()
{
	//Enable IP Conflict, Destination Unreachable, and PPPoE Interrupts 
	SPI_Write(IMR, 0xE1);
	EIMSK |= 1<<INT0; 
	//Enable interrupt on falling edge
	EICRA = 1<<ISC01; 
	sei(); 
}

void resolveIP()
{
	if(ip_addr[3] < 255)
	{
		//increment IP Address until a free address is found 
		ip_addr[3] += 1; 
	}
	W5100_Init(); 
}

// Assign I/O stream to UART
FILE uart_str = FDEV_SETUP_STREAM(uart_putch, uart_getch, _FDEV_SETUP_RW);

void Ethernet_Init()
{
		// Set the PORTD as Output:
		DDRD=0xFF;
		PORTD=0x00;
		DDRA = 0xFF; 
		// Define Output/Input Stream
		stdout = stdin = &uart_str;
		// Initial UART Peripheral
		uart_init();
		// Clear Screen
		ansi_me();
		ansi_cl();
		ansi_me();
		ansi_cl();
		uart_flush();
		// Initial the AVR ATMega168/328 SPI Peripheral
		// Set MOSI (PORTB5),SCK (PORTB7) and PORTB4 (SS) as output, others as input
		SPI_DDR = (1<<PORTB5)|(1<<PORTB7)|(1<<PORTB4);
		// CS pin is not active
		CS_PORT |= (1<<SPI_CS);
		// Enable SPI, Master Mode 0, set the clock rate fck/2
		SPCR0 = (1<<SPE0)|(1<<MSTR0);
		//SPSR0 |= (1<<SPI2X0);
		// Initial the Wiznet W5100
		printf("Wiznet W5100 Init\n");
		W5100_Init();
		// Loop forever
		for(;;){
		}
		//return 0;
}

ISR(INT0_vect)
{
	//check the status of the interrupt register 
	printf("Mask Register Value: %d\n", SPI_Read(IMR));
	unsigned char error = SPI_Read(INTR);
	printf("Interrupt Register Value: %d\n", error);
	SPI_Write(S0_IR, 0x00);
	SPI_Write(S1_IR, 0x00);
	SPI_Write(S2_IR, 0x00);
	SPI_Write(S3_IR, 0x00);
	SPI_Write(INTR, SPI_Read(INTR) | 0xE0);
	//Socket 0 Interrupt 
	if((error & (1<<S0_INT)) == 1)
	{
		unsigned char socketInt = SPI_Read(S0_IR);
		
		if((socketInt & (1<<SIR_SEND_OK))){
			printf("Send Operation Complete.\n");
		}
		else if((socketInt & (1<<SIR_TIMEOUT))){
			printf("Timeout Occurred on Socket 0!\n");
		}
		else if((socketInt & (1<<SIR_RECV))){
			printf("Receiving Data...\n");
		}
		else if((socketInt & (1<<SIR_DISCON))){
			printf("Socket Disconnected.\n");
		}
		else if((socketInt & (1<<SIR_CON))){
			printf("Socket 0 Connected.\n");
		}
		else{
			printf("Undefined Interrupt Occurred. Int Value: %d", socketInt);
		}
	}
	
	//switch(error)
	//{
		//case 0x80:
			//printf("IP Conflict! Resolving...\n\n");
			//resolveIP(); 
		//break;
		//case 0x40: 
			//printf("Destination Unreachable! Check Connections.\n\n");
		//break;
		//case 0x20: 
			//printf("PPPoE Connection Closed.\n\n");
		//break; 
		//default: 
			//printf("Unspecified Interrupt Occurred.\n\n");
		//break;  
	//}
}

