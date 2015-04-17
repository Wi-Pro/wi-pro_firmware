/*
 * IntelHex.h
 *
 * Created: 4/14/2015 1:33:09 AM
 *  Author: Brandon
 */ 


#ifndef INTELHEX_H_
#define INTELHEX_H_

//Hex Row Positions 
#define START_CODE 0 
#define BYTE_COUNT 1 
#define ADDRESS_H 2 
#define ADDRESS_L 3 
#define RECORD_TYPE 4
#define DATA_BEGIN 5 

//Record Types 
#define TYPE_DATA 0x00 
#define TYPE_END_OF_FILE 0x01

//Page Sizes 
#define PAGE_SIZE_BYTES 32 

char* getHexRow();
void hexInit();
void writeHexFileTest();





#endif /* INTELHEX_H_ */