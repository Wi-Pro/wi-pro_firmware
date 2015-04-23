/*
 * Program.h
 *
 * Created: 4/19/2015 7:45:54 PM
 *  Author: Brandon
 */ 


#ifndef PROGRAM_H_
#define PROGRAM_H_

int Program(uint32_t sigBytes); 
unsigned int selectChip(uint16_t id);
int verifySignature();
int compressFile(uint16_t length);
int checkSum(); 

#endif /* PROGRAM_H_ */