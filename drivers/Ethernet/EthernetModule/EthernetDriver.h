/*
 * EthernetDriver.h
 *
 * Created: 2/7/2015 6:11:40 PM
 *  Author: Brandon
 */ 


#ifndef ETHERNETDRIVER_H_
#define ETHERNETDRIVER_H_

#define BAUD_RATE 2400

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define CS_DDR DDRA 
#define CS_PORT PORTA 
#define ETH_CS   PORTA0  
#define MOSI PORTB5 
#define SCK PORTB7 
#define SS PORTB4 

#define WIZNET_WRITE_OPCODE 0xF0
#define WIZNET_READ_OPCODE 0x0F

// Wiznet W5100 Register Addresses
#define MR   0x0000   // Mode Register
#define GAR  0x0001   // Gateway Address: 0x0001 to 0x0004
#define SUBR 0x0005   // Subnet mask Address: 0x0005 to 0x0008
#define SAR  0x0009   // Source Hardware Address (MAC): 0x0009 to 0x000E
#define SIPR 0x000F   // Source IP Address: 0x000F to 0x0012
#define RMSR 0x001A   // RX Memory Size Register
#define TMSR 0x001B   // TX Memory Size Register
#define INTR 0x0015   //Interrupt Status Register 
#define IMR  0x0016   //Interrupt Mask Register 
#define S0_MR 0x0400 //Socket 0 Mode Register 
#define S0_CR 0x0401 //Socket 0 Command Register 
#define S0_IR 0x0402 //Socket 0 Interrupt Register
#define S1_IR 0x0502 //Socket 1 Interrupt Register
#define S2_IR 0x0602 //Socket 2 Interrupt Register
#define S3_IR 0x0702 //Socket 3 Interrupt Register 
#define S0_SR 0x0403 //Socket 0 Status Register 
#define S0_SPORT 0x0404 //Socket 0 Source Port Register 
#define S0_DIPR 0x040C //Socket 0 Destination IP Address Register 
#define S0_DPORT 0x0410 //Socket 0 Destination Port Register 
#define S0_MSS 0x0412 //Socket 0 Maximum Segment Size Register (2 Bytes)
#define S0_TX_FSR 0x0420 //Socket 0 TX Free Size Register 
#define S0_TX_RR 0x0422 //Socket 0 TX Read Pointer Register 
#define S0_TX_WR 0x0424 //Socket 0 TX Writer Pointer Register 
#define S0_RX_RSR 0x426 //Socket 0 RX Receive Size Register
#define S0_RX_RD 0x428 //Socket 0 RX Read Pointer Register
//Socket Mode Values 
#define TCP_MODE 0x01
#define IPRAW_MODE 0x03
//Socket Command Register Values 
#define OPEN 0x01 
#define LISTEN 0x02 
#define CONNECT 0x04 
#define DISCON 0x08
#define CLOSE 0x10 
#define SEND 0x20 
#define SEND_MAC 0x21 
#define SEND_KEEP 0x22
#define RECV 0x40 
//Socket Interrupt Register Positions 
#define SIR_SEND_OK 4 
#define SIR_TIMEOUT 3 
#define SIR_RECV 2 
#define SIR_DISCON 1 
#define SIR_CON 0 
//Socket Status Register Values 
#define SOCK_CLOSED 0x00 
#define SOCK_INIT 0x13
#define SOCK_LISTEN 0x14
#define SOCK_SYNSENT 0x15
#define SOCK_ESTABLISHED 0x17 
#define SOCK_CLOSE_WAIT 0x1C 
#define SOCK_UDP 0x22 
#define SOCK_IPRAW 0x32
#define SOCK_MACRAW 0x42 
#define SOCK_PPPOE 0x5F 
//Interrupt Status Register Bits (Read Only) 
#define IP_CONFLICT 7 
#define DEST_UNREACH 6 
#define PPPoE 5 
#define S3_INT 3 
#define S2_INT 2 
#define S1_INT 1 
#define S0_INT 0 

#define MAX_BUFF 65536


void uart_init(void);
void uart_flush(void);
int uart_putch(char ch,FILE *stream);
int uart_getch(FILE *stream);
void ansi_cl(void);
void ansi_me(void);
void SPI_EthernetWrite(unsigned int addr,unsigned char data);
unsigned char SPI_EthernetRead(unsigned int addr);
int Server_Connect(uint8_t socketMode, uint8_t *server_ip_addr, uint8_t *server_port, uint8_t *source_port);
void NetworkSetup(uint8_t *mac_addr, uint8_t *local_ip_addr, uint8_t *sub_mask, uint8_t *gtw_addr);
void UARTStream_Init();
void Memory_Init(); 
int SendData(const uint8_t *buffer,uint16_t bufferLength); 
int ReceiveData(uint8_t *buf,uint16_t buflen);
uint16_t ReceiveSize(void);
int getReceiveFlag();
void SocketCommand(uint8_t command); 
void SocketDisconnect();
void EnableEthernetInterrupt();


#endif /* ETHERNETDRIVER_H_ */