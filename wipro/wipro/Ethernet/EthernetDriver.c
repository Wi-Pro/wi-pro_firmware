/*
 * EthernetModule.c
 *
 * Created: 2/7/2015 6:11:09 PM
 * Author: Brandon
 * Note: This driver is based off of code published online by the Author 'rwb' on his blog, located at http://www.ermicro.com/blog/?p=1773
 */ 

#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "EthernetDriver.h"
#include "Ethernet.h"
#include "../RAM/RAMDriver.h"
#include "../RAM/MemoryMap.h"
	
unsigned int S0_RX_BASE;
unsigned int S0_RX_MASK;
unsigned int S0_TX_BASE;
unsigned int S0_TX_MASK;

int receiveFlag = 0; 

unsigned char gl_server_ip_addr[4] = {104,131,36,80};
unsigned char gl_server_port[2] = {00,80};
unsigned char gl_source_port[2] = {00,80};

//void uart_init(void)
//{
	//UBRR0H = (((F_CPU/BAUD_RATE)/16)-1)>>8;	// set baud rate
	//UBRR0L = (((F_CPU/BAUD_RATE)/16)-1);
	//UCSR0B = (1<<RXEN0)|(1<<TXEN0); 		// enable Rx & Tx
	//UCSR0C =  (1<<UCSZ01)|(1<<UCSZ00);  	// config USART; 8N1
//}

void SPI_EthernetWrite(unsigned int addr,unsigned char data)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<ETH_CS);
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
	CS_PORT |= (1<<ETH_CS);
}

unsigned char SPI_EthernetRead(unsigned int addr)
{
	// Activate the CS pin
	CS_PORT &= ~(1<<ETH_CS);
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
	CS_PORT |= (1<<ETH_CS);
	return(SPDR0);
}

void NetworkSetup(uint8_t *mac_addr, uint8_t *local_ip_addr, uint8_t *sub_mask, uint8_t *gtw_addr)
{
	
	CS_DDR |= (1<<ETH_CS);
	// Set MOSI, SCK and SS as output, others as input
	SPI_DDR = (1<<MOSI)|(1<<SCK)|(1<<SS);
	// CS pin is not active
	CS_PORT |= (1<<ETH_CS);
	// Enable SPI, Master Mode 0, set the clock rate fck/2
	SPCR0 = (1<<SPE0)|(1<<MSTR0);
	// Initial the Wiznet W5100
	//printf("Wiznet W5100 Init\n");
	// Ethernet Setup
	//printf("Enterting Ethernet Setup\n");
	// Setting the Wiznet W5100 Mode Register: 0x0000
	SPI_EthernetWrite(MR,0x80); 
	_delay_ms(1);
	/*printf("Reading MR: %d\n\n",SPI_EthernetRead(MR));
	// Setting the Wiznet W5100 Gateway Address
	//printf("Setting Gateway Address %d.%d.%d.%d\n",gtw_addr[0],gtw_addr[1],\
	gtw_addr[2],gtw_addr[3]);*/
	SPI_EthernetWrite(GAR + 0,gtw_addr[0]);
	SPI_EthernetWrite(GAR + 1,gtw_addr[1]);
	SPI_EthernetWrite(GAR + 2,gtw_addr[2]);
	SPI_EthernetWrite(GAR + 3,gtw_addr[3]);
	_delay_ms(1);
	//printf("Reading GAR: %d.%d.%d.%d\n\n",SPI_EthernetRead(GAR + 0),SPI_EthernetRead(GAR + 1),\
	SPI_EthernetRead(GAR + 2),SPI_EthernetRead(GAR + 3));
	
	// Setting the Wiznet W5100 Source Address Register
	//printf("Setting Source Address %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",mac_addr[0],mac_addr[1],\
	mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	SPI_EthernetWrite(SAR + 0,mac_addr[0]);
	SPI_EthernetWrite(SAR + 1,mac_addr[1]);
	SPI_EthernetWrite(SAR + 2,mac_addr[2]);
	SPI_EthernetWrite(SAR + 3,mac_addr[3]);
	SPI_EthernetWrite(SAR + 4,mac_addr[4]);
	SPI_EthernetWrite(SAR + 5,mac_addr[5]);
	_delay_ms(1);
	//printf("Reading SAR: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n\n",SPI_EthernetRead(SAR + 0),SPI_EthernetRead(SAR + 1),\
	SPI_EthernetRead(SAR + 2),SPI_EthernetRead(SAR + 3),SPI_EthernetRead(SAR + 4),SPI_EthernetRead(SAR + 5));
	
	// Setting the Wiznet W5100 Sub Mask Address
	//printf("Setting Sub Mask Address %d.%d.%d.%d\n",sub_mask[0],sub_mask[1],\
	sub_mask[2],sub_mask[3]);
	SPI_EthernetWrite(SUBR + 0,sub_mask[0]);
	SPI_EthernetWrite(SUBR + 1,sub_mask[1]);
	SPI_EthernetWrite(SUBR + 2,sub_mask[2]);
	SPI_EthernetWrite(SUBR + 3,sub_mask[3]);
	_delay_ms(1);
	//printf("Reading SUBR: %d.%d.%d.%d\n\n",SPI_EthernetRead(SUBR + 0),SPI_EthernetRead(SUBR + 1),\
	SPI_EthernetRead(SUBR + 2),SPI_EthernetRead(SUBR + 3));
	
	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	//printf("Setting IP Address %d.%d.%d.%d\n",local_ip_addr[0],local_ip_addr[1],\
	local_ip_addr[2],local_ip_addr[3]);
	SPI_EthernetWrite(SIPR + 0,local_ip_addr[0]);
	SPI_EthernetWrite(SIPR + 1,local_ip_addr[1]);
	SPI_EthernetWrite(SIPR + 2,local_ip_addr[2]);
	SPI_EthernetWrite(SIPR + 3,local_ip_addr[3]);
	_delay_ms(1);
	
	//printf("Reading SIPR: %d.%d.%d.%d\n\n",SPI_EthernetRead(SIPR + 0),SPI_EthernetRead(SIPR + 1),\
	SPI_EthernetRead(SIPR + 2),SPI_EthernetRead(SIPR + 3));
	//printf("Done Wiznet W5100 Initialized!\n");
	//Memory_Init(); 
}

int Server_Connect(uint8_t socketMode, uint8_t *server_ip_addr, uint8_t *server_port, uint8_t *source_port)
{
	do 
	{
		printf("SPI SR Status: %d\n", SPI_EthernetRead(S0_SR));
		SPI_EthernetWrite(S0_CR, CLOSE);
		//SPI_Write(S0_MR, TCP_MODE);
		SPI_EthernetWrite(S0_MR, socketMode);
		//printf("Setting Source Port on Socket 0 %d%d\n",source_port[0],source_port[1]);
		SPI_EthernetWrite(S0_SPORT + 0, source_port[0]);
		SPI_EthernetWrite(S0_SPORT + 1, source_port[1]);
		//printf("Reading Source Port on Socket 0 %d%d\n\n",SPI_EthernetRead(S0_SPORT),SPI_EthernetRead(S0_SPORT + 1));
		
		//printf("Setting Socket 0 to Open\n\n");
		SPI_EthernetWrite(S0_CR, OPEN);
		//printf("SPI SR Status: %d\n", SPI_EthernetRead(S0_SR));
		
	} while (SPI_EthernetRead(S0_SR) != SOCK_INIT);
	
	//Write Server IP to Socket 0 IP Register
	//printf("Setting Server IP Address %d.%d.%d.%d\n",server_ip_addr[0],server_ip_addr[1],\
	server_ip_addr[2],server_ip_addr[3]); 
	SPI_EthernetWrite(S0_DIPR + 0,server_ip_addr[0]);
	SPI_EthernetWrite(S0_DIPR + 1, server_ip_addr[1]);
	SPI_EthernetWrite(S0_DIPR + 2,server_ip_addr[2]);
	SPI_EthernetWrite(S0_DIPR + 3, server_ip_addr[3]);
	//printf("Reading S0_DIPR: %d.%d.%d.%d\n\n",SPI_EthernetRead(S0_DIPR + 0),SPI_EthernetRead(S0_DIPR + 1),\
	SPI_EthernetRead(S0_DIPR + 2),SPI_EthernetRead(S0_DIPR + 3));
	//printf("SPI SR Status: 0x%02x\n", SPI_EthernetRead(S0_SR));
	
	//Write Server Port to Socket 0 IP Register 
	//printf("Setting Server Port\n");
	SPI_EthernetWrite(S0_DPORT + 0, server_port[0]); 
	SPI_EthernetWrite(S0_DPORT + 1, server_port[1]);
	//printf("Reading S0_DPORT: %d%d\n\n",SPI_EthernetRead(S0_DPORT + 0),SPI_EthernetRead(S0_DPORT + 1)); 
	//printf("SPI SR Status: 0x%02x\n", SPI_EthernetRead(S0_SR));
	
	//Try to connect 
	SPI_EthernetWrite(S0_CR, CONNECT);
	//while((SPI_EthernetRead(S0_IR) & 0x1F) != 0); 
	//while((SPI_EthernetRead(S0_SR) != SOCK_ESTABLISHED)); 

	//Check to see if connection has been established 
	unsigned int delayCount = 0; 
	while(SPI_EthernetRead(S0_SR) != SOCK_ESTABLISHED)
	{
		_delay_us(300); 
		delayCount += 1; 
		//Every 3 seconds
		if(delayCount%10000 == 0)
		{
			printf("Trying to Establish Connection...\n");
			//delayCount = 0; 
			//15 seconds
			if(delayCount > 50000)
			{
				printf("Timeout, disconnecting!\n"); 
				SPI_EthernetWrite(S0_CR, CLOSE); 
				return 0; 
			}
		}
	}
	
	uint8_t sr_status = SPI_EthernetRead(S0_SR);
	if(sr_status == SOCK_ESTABLISHED)
	{
		//printf("SPI SR Status: 0x%02x\n", sr_status);
		printf("Connection Succeeded!\n\n");
		return 1; 
	}
	//Connection Failed 
	else
	{
		//printf("SPI SR Status: 0x%02x\n", sr_status);
		printf("Could not connect. Exiting...\n\n");
		return 0; 
	}
}

void MemoryInit()
{
	//assign 8kb rx memory to socket 0 
	SPI_EthernetWrite(RMSR, 0x03);
	//assign 8kb tx memory to socket 0 
	SPI_EthernetWrite(TMSR, 0x03);	
	//Equal 2K buffers 
	//S0_TX_MASK = 0x7FF;
	//S0_TX_BASE = 0x4000;
	//S0_RX_MASK = 0x07FF;
	//S0_RX_BASE = 0x6000;
	//
	//8k RX and TX Socket 0 
	S0_TX_MASK = 0x1FFF;
	S0_TX_BASE = 0x4000;
	S0_RX_MASK = 0x1FFF;
	S0_RX_BASE = 0x6000;
}

int SendData(const uint8_t *buffer,uint16_t bufferLength, int externRAM)
{
		
	uint16_t ptr,offaddr,realaddr,txsize,timeout,i,data;
	printf("SR: 0x%02x\n", SPI_EthernetRead(S0_SR));
	if(SPI_EthernetRead(S0_SR) != SOCK_ESTABLISHED){
		while(!Server_Connect(TCP_MODE, gl_server_ip_addr, gl_server_port, gl_source_port));
	}
	//#if _DEBUG_MODE
	//printf("Send Size: %d\n",bufferLength);
	//#endif
	// Make sure the TX Free Size Register is available
	txsize = SPI_EthernetRead(S0_TX_FSR);
	txsize = (((txsize & 0x00FF) << 8 ) + SPI_EthernetRead(S0_TX_FSR + 1));
	//#if _DEBUG_MODE
	//printf("TX Free Size: %d\n",txsize);
	//#endif
	timeout = 0;
	//Writing our buffer to the transmit buffer
	while (txsize < bufferLength) {
		_delay_ms(1);
		txsize = SPI_EthernetRead(S0_TX_FSR);
		txsize = (((txsize & 0x00FF) << 8 ) + SPI_EthernetRead(S0_TX_FSR + 1));
		printf("TX Size: %d", txsize);
		// Timeout for approx 1000 ms
		if (timeout++ > 1000) {
			//#if _DEBUG_MODE
			//printf("TX Free Size Error!\n");
			//#endif
			// Disconnect the connection
			SocketCommand(DISCON);
			while(SPI_EthernetRead(S0_CR));
			return 0;
		}
	}

	// Read the Tx Write Pointer
	ptr = SPI_EthernetRead(S0_TX_WR);
	offaddr = (((ptr & 0x00FF) << 8 ) + SPI_EthernetRead(S0_TX_WR + 1));
	unsigned int startaddr = (((SPI_EthernetRead(S0_TX_RR) << 8) + SPI_EthernetRead(S0_TX_RR + 1)));
	//#if _DEBUG_MODE
	//printf("TX Buffer: %x\n",offaddr);
	//unsigned int realWR = S0_TX_BASE + (offaddr & S0_TX_MASK);
	//unsigned int realRR = S0_TX_BASE + (startaddr & S0_TX_MASK);
	//printf("Real WR: %d\n", realWR);
	//printf("Real RR: %d\n", realRR); 
	//#endif 
	i = 0; 
	data = 0; 
	printf("Sending...\n\n");
	while(i < bufferLength) {

		// Calculate the real W5100 physical Tx Buffer Address
		realaddr = S0_TX_BASE + (offaddr & S0_TX_MASK);
		//printf("TX Real Address: %d\n", realaddr);
		// Copy the application data to the W5100 Tx Buffer
		if(externRAM)
		{
			//Grab the byte from external RAM and write it to the Wiznet TX Buffer 
			data = RAMReadByte(WIFI_QSTRING_ADDRESS + i);
			//_delay_us(10);
			//printf("%c", data);
			SPI_EthernetWrite(realaddr, data);
			//_delay_us(10);
		}
		else
		{
			//printf("%c", *buffer);
			SPI_EthernetWrite(realaddr,*buffer);	
		}
		offaddr++;
		buffer++;
		i++; 
	}
		SPI_EthernetWrite(realaddr + 1, 0x00);

	// Increase the S0_TX_WR value, so it points to the next transmit
	//printf("Offset After: %d\n", offaddr);
	ptr = SPI_EthernetRead(S0_TX_WR);
	//printf("WR Low: %d\n", SPI_EthernetRead(S0_TX_WR)); 
	//printf("WR High: %d\n", SPI_EthernetRead(S0_TX_WR + 1)); 
	//offaddr = (((ptr & 0x00FF) << 8 ) + SPI_Read(S0_TX_WR + 1));
	//realWR = S0_TX_BASE + (offaddr & S0_TX_MASK);
	//printf("Real WR: %d\n", realWR);
	//printf("%d\n", (offaddr & 0xFF00) >> 8);
	//printf("%d\n", (offaddr & 0x00FF));
	//SPI_Write(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
	SPI_EthernetWrite(S0_TX_WR, 0xFF);
	SPI_EthernetWrite(S0_TX_WR + 1,(offaddr & 0x00FF));
	//printf("%d\n", (offaddr & 0xFF00) >> 8);
	//printf("%d\n", (offaddr & 0x00FF));
	//ptr = SPI_Read(S0_TX_WR);
	
	//Debug 
	//printf("WR Low: %d\n", SPI_EthernetRead(S0_TX_WR));
	//printf("WR High: %d\n", SPI_EthernetRead(S0_TX_WR + 1));
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
	while(SPI_EthernetRead(S0_CR));
	//while(!(SPI_EthernetRead(S0_IR) & (1<<SIR_SEND_OK))); 
	return 1;
}

int ReceiveData(uint16_t RAMAddress, char* buffer, uint16_t dataLength, int header, int externRAM)
{
	uint16_t ptr,offaddr,realaddr;
	//uint16_t RAMAddress = 0; 
	char tempvar; 
	//uint8_t header = 1; 
	uint8_t headerCount = 0; 
	uint16_t timeCount = 0; 
	// If the request size > MAX_BUF,just truncate it
	if (dataLength > MAX_BUFF)
		dataLength = MAX_BUFF - 2;
		
	while(1)
	{
		if(SPI_EthernetRead(S0_SR) == SOCK_ESTABLISHED)
		{
			// Read the Rx Read Pointer
			ptr = SPI_EthernetRead(S0_RX_RD);
			//printf("RX Read Pointer: %d\n", ptr);
			offaddr = (((ptr & 0x00FF) << 8 ) + SPI_EthernetRead(S0_RX_RD + 1));
			//#if _DEBUG_MODE
			//printf("RX Buffer: %x\n",offaddr);
			//#endif
			//RAMWriteByte(0x32, 0000);
			while(dataLength) {
				realaddr = S0_RX_BASE + (offaddr & S0_RX_MASK);
				//printf("Real Address: %d\n", realaddr);
				tempvar = SPI_EthernetRead(realaddr);
				//printf("Buffer: %c\n", tempvar);
				if(tempvar == '\n' && header)
				{
					//printf("Found New Line!\n");
					headerCount++; 
					if(headerCount >= 10)
						header = 0; 
				}
				else if(!header)
				{
					//printf("Write to RAM!\n");
					//printf("Writing to RAM: %c @ address %d\n", tempvar, RAMAddress); 
					if(externRAM){
						RAMWriteByte(tempvar, RAMAddress);	
						RAMAddress++;
					}
					else{
						//printf("Writing to Buffer\n"); 
						*buffer = tempvar; 
						buffer++; 
					}
					dataLength--;
				}
				offaddr++;
			}
			*buffer='\0';        // String terminated character
			//printf("%s", buffer);

			// Increase the S0_RX_RD value, so it point to the next receive
			SPI_EthernetWrite(S0_RX_RD,(offaddr & 0xFF00) >> 8 );
			SPI_EthernetWrite(S0_RX_RD + 1,(offaddr & 0x00FF));

			// Now Send the RECV command
			SocketCommand(RECV);
			_delay_us(5);    // Wait for Receive Process
			break; 
		}
		else if(timeCount > MAX_TIME_COUNT)
		{
			printf("Ethernet Receive Data Timeout!\n");
			printf("Stats Reg: 0x%02x", SPI_EthernetRead(S0_SR));
			break; 
		}
		else
		{
			timeCount++; 
			_delay_ms(10);
		}
		
	}	

	return 1;
}

uint16_t ReceiveSize(void)
{
	return ((SPI_EthernetRead(S0_RX_RSR) & 0x00FF) << 8 ) + SPI_EthernetRead(S0_RX_RSR + 1);
}

void SocketCommand(uint8_t command) 
{
	SPI_EthernetWrite(S0_CR, command);
}

void SocketDisconnect()
{
	SocketCommand(DISCON);
	while(SPI_EthernetRead(S0_CR));
}

void EnableEthernetInterrupt()
{
	//Enable IP Conflict, Destination Unreachable, and PPPoE Interrupts 
	SPI_EthernetWrite(IMR, 0xE1);
	EIMSK |= 1<<INT0; 
	//Enable interrupt on falling edge
	EICRA = 1<<ISC01; 
	sei(); 
}

void ResolveIP(uint8_t local_ip_addr[4])
{
	if(local_ip_addr[3] < 255)
	{
		//increment IP Address until a free address is found 
		local_ip_addr[3] += 1; 
	}
	//Ethernet_Init(mac_addr, local_ip_addr, sub_mask, gtw_addr); 
}

// Assign I/O stream to UART
//FILE uart_str = FDEV_SETUP_STREAM(uart_putch, uart_getch, _FDEV_SETUP_RW);

int getReceiveFlag()
{
	return receiveFlag; 
}

//ISR(INT0_vect)
//{
	//cli(); 
	////ReceiveData(8000);
	////char* dummyBuffer; 
	////check the status of the interrupt register 
	//printf("Mask Register Value: %d\n", SPI_EthernetRead(IMR));
	//unsigned char error = SPI_EthernetRead(INTR);
	//printf("Interrupt Register Value: %d\n", error);
	////SPI_EthernetWrite(S0_IR, 0xFF);
	////SPI_EthernetWrite(S1_IR, 0x00);
	////SPI_EthernetWrite(S2_IR, 0x00);
	////SPI_EthernetWrite(S3_IR, 0x00);
//
	////SPI_EthernetWrite(INTR, SPI_EthernetRead(INTR) | 0xE0);
	////Socket 0 Interrupt 
	//if((error & (1<<S0_INT)) == 1)
	//{
		//unsigned char socketInt = SPI_EthernetRead(S0_IR);
		//SPI_EthernetWrite(S0_IR, 0xFF); 
		//
		//if((socketInt & (1<<SIR_SEND_OK))){
			//printf("Send Operation Complete.\n");
		//}
		//else if((socketInt & (1<<SIR_TIMEOUT))){
			//printf("Timeout Occurred on Socket 0!\n");
		//}
		//else if((socketInt & (1<<SIR_RECV))){
			////ReceiveData(8000, 0);
			//SocketDisconnect();
			//receiveFlag = 1; 
			//printf("Receiving Data...\n");
		//}
		//else if((socketInt & (1<<SIR_DISCON))){
			//printf("Socket Disconnected.\n");
		//}
		//else if((socketInt & (1<<SIR_CON))){
			//printf("Socket 0 Connected.\n");
		//}
		//else{
			//printf("Undefined Interrupt Occurred. Int Value: %d", socketInt);
		//}
		////sei(); 
	//}
//}

