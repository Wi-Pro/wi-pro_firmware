/*
 * Wifi.h
 *
 * Created: 3/15/2015 1:03:33 AM
 *  Author: Brandon
 */ 


#ifndef WIFI_H_
#define WIFI_H_

//Initialization Commands 
//structs
struct connStatus{
	int networkUp; 
	int machineMode; 
	char* SSID; 
	char* password; 
	};
//commands 
#define SCAN "scan"
#define HTTP_GET "http_get "
#define TCP_CLIENT "tcp_client"
#define STREAM_READ "stream_read"
#define STREAM_CLOSE "stream_close all"

//variables
#define SYSTEM_PRINT_LEVEL "system.print_level"
#define SYSTEM_CMD_HEADER "system.cmd.header_enabled"
#define SYSTEM_CMD_PROMPT "system.cmd.prompt_enabled"
#define SYSTEM_CMD_ECHO "system.cmd.echo"
#define WLAN_SSID "wlan.ssid"
#define WLAN_PWD "wlan.passkey"

//values
#define ONE "1" 
#define ZERO "0" 
#define ON "on"
#define OFF "off"

void wifiInit(); 
void setMachineMode(); 
void setHumanMode(); 
char* networkScan(); 
int networkConnect(char* SSID, char* password); 
int serverConnect(char* serverDNS, unsigned char* port);
uint16_t networkQueryString(char* filepath);
char* getFileWifi(char* filepath, int externRAM, uint32_t RAMAddress, int multiReceive);
void updateFileWifi(char* filepath);


#endif /* WIFI_H_ */