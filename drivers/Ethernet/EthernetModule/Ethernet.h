/*
 * Ethernet.h
 *
 * Created: 3/28/2015 11:23:40 PM
 *  Author: Brandon
 */ 


#ifndef ETHERNET_H_
#define ETHERNET_H_

struct network{
	unsigned char mac_addr[5];
	unsigned char local_ip_addr[4];
	unsigned char sub_mask[4];
	unsigned char gtw_addr[4];
	unsigned char server_ip_addr[4];
	unsigned char server_port[2];
	unsigned char source_port[2]; 
};

void ethernetInit();
unsigned char* getRequestEthernet(char* filepath);
void postRequestEthernet(char* filepath, char* data);
void getHexFile(uint8_t numOfFiles);


#endif /* ETHERNET_H_ */