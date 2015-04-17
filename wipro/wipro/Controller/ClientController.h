/*
 * ClientController.h
 *
 * Created: 4/6/2015 2:05:38 AM
 *  Author: Brandon
 */ 


#ifndef CLIENTCONTROLLER_H_
#define CLIENTCONTROLLER_H_
//url 
#define URL "http://www.wi-pro.us"
//Server Commands & Parameters 
#define GET "GET "
#define WIPRO_ID "wiproid=1000000000"
#define SSID_ID "&ssid="
//Controllers  
#define SSID_UPDATE "/data/updatessids?"
#define CLEAR_FLAG "/data/clearflags?"
//Server Files
#define FLAG_FILE "/uploads/hex/1000000000/flagfile.txt"
#define HEX_FILE "/uploads/hex/1000000000/default.hex"

//Flag positions
#define ETHERNET 0 
#define WIFI 1
#define PROGRAM 2 
#define NETWORK_SCAN 3 
#define NETWORK_CONNECT 4
#define FLAG_ARRAY_LENGTH 5 


void sendAvailableNetworks();
int getFlagStatus();
void pollingInit();
int clearFlags(); 
int getHexFile(); 



#endif /* CLIENTCONTROLLER_H_ */