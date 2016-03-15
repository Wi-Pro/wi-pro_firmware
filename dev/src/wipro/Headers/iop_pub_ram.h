/*
 * RAMDriver.h
 *
 * Created: 3/25/2015 8:40:52 PM
 *  Author: Brandon
 */ 


#ifndef RAMDRIVER_H_
#define RAMDRIVER_H_

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define RAM_PORT PORTG
#define RAM_DDR DDRG 
#define RAM_CS 3

#define MOSI 2
#define SCK 1
#define SS 0

#define READ 0x03
#define WRITE 0x02
#define RDSR 0x05 
#define WRSR 0x01 

//Modes 
#define BYTE 0x00
#define PAGE 0x80
#define SEQ 0x40



int getRAMStatus(); 
void setRAMStatus(char mode);
uint16_t RAMWrite(char* data, uint16_t startAddress, uint16_t length);
void RAMWriteByte(char data, uint32_t address);
char RAMReadByte(uint32_t address);
void HWM_spi_read_addr(uint32_t address);
char* RAMRead(uint32_t startAddress, uint16_t length, char* buffer);
void RAMPrint(uint32_t startAddress, uint16_t length);




#endif /* RAMDRIVER_H_ */