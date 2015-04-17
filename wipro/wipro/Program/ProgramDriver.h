/*
 * ProgramDriver.h
 *
 * Created: 4/13/2015 8:47:17 PM
 *  Author: Brandon
 */ 


#ifndef PROGRAMDRIVER_H_
#define PROGRAMDRIVER_H_

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define MISO 3
#define MOSI 2
#define SCK 1
#define SS 0


#define SR_CNTRL_PORT PORTJ
#define SR_CNTRL_DDR DDRJ
#define SR_RESET 5
#define MAXCS 4
#define SROE 3

//#define LT_OE_DDR DDRG
//#define LT_OE_PORT PORTG
//#define LTOE 5

#define SRCS_PORT PORTB
#define SRCS_DDR DDRB
#define SRCS 7

#define WR_DDR DDRJ
#define WR_PORT PORTJ
#define FPGAWR 2

#define RDY_BSY 0

#define DATA_PORT PORTL
#define DATA_PIN PINL
#define DATA_DDR DDRL

//Commands Byte Bit Coding  
#define CHIP_ERASE 0x80 
#define WRITE_FUSE_BITS 0x40 
#define WRITE_LOCK_BITS 0x20 
#define WRITE_FLASH 0x10 
#define WRITE_EEPROM 0x11 
#define READ_SIG_BYTE 0x08
#define READ_FUSE_LOCK_BITS 0x40 
#define READ_FLASH 0x02 
#define READ_EEPROM 0x03 


//Control Port Pins
#define CONTROL_PORT PORTD
#define CONTROL_PIN PIND
#define CONTROL_DDR DDRD

#define XTAL1 0
#define OE 1
#define WR 2
#define BS1_PAGEL 3
#define XA0 4
#define XA1_BS2 5
#define PAGEL 6
#define BS2 7

#define RDY_BSY_DDR DDRC
#define RDY_BSY_PORT PORTC
#define RDY_BSY_In PINC

#define LED_DDR DDRG
#define LED_PORT PORTG
#define LED_Green 0
#define LED_Yellow 1
#define LED_Red 2

void USB_UART2_Initialization();
void USB_UART2_Out(unsigned char TransmissionValue);
unsigned char USB_UART2_In();
void HexToASCII(unsigned char DataValue);

void USB_UART0_Initialization();
void USB_UART0_Out(unsigned char TransmissionValue);
unsigned char USB_UART0_In();

void SPI_Switching_Circuitry_Init();
void SPI_Switching_Circuitry_Write(unsigned char SPI_Data);

void ProgInit();
void EnableProgMode(unsigned char TargetMicrocontroller);
void ExitParallelProgrammingMode();
char* ReadSignatureBytes();
void ChipErase();
void ReadFlash();
void ProgramFlash();
void ProgramFlashTest();
void EndPageProgramming();



#endif /* PROGRAMDRIVER_H_ */