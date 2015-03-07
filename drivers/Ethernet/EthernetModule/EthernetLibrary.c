/*
 * EthernetModule.c
 *
 * Created: 2/7/2015 6:11:09 PM
 * Author: Brandon
 * Note: This driver is based off of code published online by the Author 'rwb' on his blog, located at http://www.ermicro.com/blog/?p=1773
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "EthernetLibrary.h"

unsigned char receiveMessage[500]; 

unsigned char mac_addr[] = {0x50,0xE5,0x49,0xBF,0x91,0x40};
unsigned char ip_addr[] = {192,168,1,5};
unsigned char sub_mask[] = {255,255,255,0};
unsigned char gtw_addr[] = {192,168,1,1};
	
unsigned char server_ip_addr[] = {104,131,36,80};
//google
//unsigned char server_ip_addr[] = {198,41,208,138};
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
	// Set MOSI, SCK and SS as output, others as input
	SPI_DDR = (1<<MOSI)|(1<<SCK)|(1<<SS);
	// CS pin is not active
	CS_PORT |= (1<<SPI_CS);
	// Enable SPI, Master Mode 0, set the clock rate fck/2
	SPCR0 = (1<<SPE0)|(1<<MSTR0);
	// Initial the Wiznet W5100
	printf("Wiznet W5100 Init\n");
	// Ethernet Setup
	printf("Enterting Ethernet Setup\n");
	// Setting the Wiznet W5100 Mode Register: 0x0000
	SPI_Write(MR,0x80); 
	_delay_ms(1);
	printf("Reading MR: %d\n\n",SPI_Read(MR));
	// Setting the Wiznet W5100 Gateway Address
	printf("Setting Gateway Address %d.%d.%d.%d\n",gtw_addr[0],gtw_addr[1],\
	gtw_addr[2],gtw_addr[3]);
	
	SPI_Write(GAR + 0,gtw_addr[0]);
	SPI_Write(GAR + 1,gtw_addr[1]);
	SPI_Write(GAR + 2,gtw_addr[2]);
	SPI_Write(GAR + 3,gtw_addr[3]);
	_delay_ms(1);
	printf("Reading GAR: %d.%d.%d.%d\n\n",SPI_Read(GAR + 0),SPI_Read(GAR + 1),\
	SPI_Read(GAR + 2),SPI_Read(GAR + 3));
	
	// Setting the Wiznet W5100 Source Address Register
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
	
	// Setting the Wiznet W5100 Sub Mask Address
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

	printf("Done Wiznet W5100 Initialized!\n");
	//enableEthernetInterrupt(); 
	Memory_Init(); 
	Server_Connect(TCP_MODE);
	//unsigned char message[100] = "GET /uploads/hex/2222222222/Toggle.hex HTTP/1.1"; 
	//unsigned char message[] = "GET http://www.wi-pro.us/uploads/hex/11111111111/default.hex HTTP/1.1 \nHost: www.wi-pro.us \n\n";
	unsigned char message[] = "POST /data?t=Hello%20World HTTP/1.1\nhost: www.wi-pro.us\n\n";
	unsigned char* messagePointer = message; 

	//unsigned char receiveMessage[] = "";

	unsigned char* recMessagePointer = receiveMessage;
	//http://104.131.36.80/uploads/hex/2222222222/Toggle.hex
	//messagePointer = "GET /uploads/hex/2222222222/Toggle.hex";
	printf("Sent Message: %s\n", message);
	SendData(messagePointer, strlen((char *)messagePointer)); 
	ReceiveData(recMessagePointer, 100);
	printf("Receive Size: %d\n", ReceiveSize());
	printf("Message: %s", recMessagePointer);
	SocketDisconnect();
}

int Server_Connect(uint8_t socketMode)
{
	do 
	{
		//printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		SPI_Write(S0_CR, CLOSE);
		//SPI_Write(S0_MR, TCP_MODE);
		SPI_Write(S0_MR, socketMode);
		printf("Setting Source Port on Socket 0 %d%d\n",source_port[0],source_port[1]);
		SPI_Write(S0_SPORT + 0, source_port[0]);
		SPI_Write(S0_SPORT + 1, source_port[1]);
		printf("Reading Source Port on Socket 0 %d%d\n\n",SPI_Read(S0_SPORT),SPI_Read(S0_SPORT + 1));
		
		printf("Setting Socket 0 to Open\n\n");
		SPI_Write(S0_CR, OPEN);
		printf("SPI SR Status: %d\n", SPI_Read(S0_SR));
		
	} while (SPI_Read(S0_SR) != SOCK_INIT && SPI_Read(S0_SR) != SOCK_IPRAW);
	
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
	while(SPI_Read(S0_SR) == SOCK_SYNSENT)
	{
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
	//Equal 2K buffers 
	S0_TX_MASK = 0x7FF;
	S0_TX_BASE = 0x4000;
	S0_RX_MASK = 0x07FF;
	S0_RX_BASE = 0x6000;
	//
	//8k RX and TX Socket 0 
	//S0_TX_MASK = 0x1FFF;
	//S0_TX_BASE = 0x4000;
	//S0_RX_MASK = 0x1FFF;
	//S0_RX_BASE = 0x6000;
}

int SendData(const uint8_t *buffer,uint16_t bufferLength)
{
	uint16_t ptr,offaddr,realaddr,txsize,timeout;
	
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
			SocketCommand(DISCON);
			while(SPI_Read(S0_CR));
			return 0;
		}
	}

	// Read the Tx Write Pointer
	ptr = SPI_Read(S0_TX_WR);
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_TX_WR + 1));
	unsigned int startaddr = (((SPI_Read(S0_TX_RR) << 8) + SPI_Read(S0_TX_RR + 1)));
	//#if _DEBUG_MODE
	printf("TX Buffer: %x\n",offaddr);
	unsigned int realWR = S0_TX_BASE + (offaddr & S0_TX_MASK);
	unsigned int realRR = S0_TX_BASE + (startaddr & S0_TX_MASK);
	printf("Real WR: %d\n", realWR);
	printf("Real RR: %d\n", realRR); 
	//#endif
	unsigned int bufferLTemp = bufferLength;
	unsigned int bufferTemp = buffer; 
	unsigned int offaddrTemp = offaddr;  
	
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
		SPI_Write(realaddr + 1, 0x00);

	// Increase the S0_TX_WR value, so it point to the next transmit
	printf("Offset After: %d\n", offaddr);
	ptr = SPI_Read(S0_TX_WR);
	printf("WR Low: %d\n", SPI_Read(S0_TX_WR)); 
	printf("WR High: %d\n", SPI_Read(S0_TX_WR + 1)); 
	//offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_TX_WR + 1));
	//realWR = S0_TX_BASE + (offaddr & S0_TX_MASK);
	//printf("Real WR: %d\n", realWR);
	printf("%d\n", (offaddr & 0xFF00) >> 8);
	printf("%d\n", (offaddr & 0x00FF));
	//SPI_Write(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
	SPI_Write(S0_TX_WR, 0xFF);
	SPI_Write(S0_TX_WR + 1,(offaddr & 0x00FF));
	printf("%d\n", (offaddr & 0xFF00) >> 8);
	printf("%d\n", (offaddr & 0x00FF));
	//ptr = SPI_Read(S0_TX_WR);
	
	//Debug 
	printf("WR Low: %d\n", SPI_Read(S0_TX_WR));
	printf("WR High: %d\n", SPI_Read(S0_TX_WR + 1));
	//offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_TX_WR + 1));
	//startaddr = (((SPI_Read(S0_TX_RR) << 8) + SPI_Read(S0_TX_RR + 1)));
	//realWR = S0_TX_BASE + (offaddr & S0_TX_MASK);
	//realRR = S0_TX_BASE + (startaddr & S0_TX_MASK);
	//printf("Real WR: %d\n", realWR);
	//printf("Real RR: %d\n", realRR);
	
	// Now Send the SEND command
	SocketCommand(SEND);
	//printf("WR Low: %d\n", SPI_Read(S0_TX_WR));
	//printf("WR High: %d\n", SPI_Read(S0_TX_WR + 1));
	// Wait for Sending Process
	while(SPI_Read(S0_CR));

	return 1;
}

int ReceiveData(uint8_t *buffer,uint16_t bufferLength)
{
	uint16_t ptr,offaddr,realaddr;

	// If the request size > MAX_BUF,just truncate it
	if (bufferLength > MAX_BUFF)
	bufferLength = MAX_BUFF - 2;
	// Read the Rx Read Pointer
	ptr = SPI_Read(S0_RX_RD);
	//printf("RX Read Pointer: %d\n", ptr);
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
	SocketCommand(RECV);
	_delay_us(5);    // Wait for Receive Process

	return 1;
}

uint16_t ReceiveSize(void)
{
	return ((SPI_Read(S0_RX_RSR) & 0x00FF) << 8 ) + SPI_Read(S0_RX_RSR + 1);
}

void SocketCommand(uint8_t command) 
{
	SPI_Write(S0_CR, command);
}

void SocketDisconnect()
{
	SocketCommand(DISCON);
	while(SPI_Read(S0_CR));
}

void EnableEthernetInterrupt()
{
	//Enable IP Conflict, Destination Unreachable, and PPPoE Interrupts 
	SPI_Write(IMR, 0xE1);
	EIMSK |= 1<<INT0; 
	//Enable interrupt on falling edge
	EICRA = 1<<ISC01; 
	sei(); 
}

void ResolveIP()
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
		W5100_Init();
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

