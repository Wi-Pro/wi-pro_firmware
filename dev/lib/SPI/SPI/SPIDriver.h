/*
 * SPIDriver.h
 *
 * Created: 3/1/2015 3:44:21 PM
 *  Author: Brandon
 */ 

//SPI Definitions 
#ifndef SPIDRIVER_H_
#define SPIDRIVER_H_

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define CS_PORT PORTA
#define SPI_CS   PORTA0

#define MOSI PORTB5 
#define SCK PORTB7 
#define SS PORTB4 

//#define WRITE_OPCODE 0xF0
//#define READ_OPCODE 0x0F

void SPI_Init();
void SPI_Write(unsigned int addr,unsigned char data, unsigned char writeOpcode);
unsigned char SPI_Read(unsigned int addr, unsigned char readOpcode);


#endif /* SPIDRIVER_H_ */