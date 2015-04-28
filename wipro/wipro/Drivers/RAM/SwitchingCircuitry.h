/*
 * SwitchingCircuitry.h
 *
 * Created: 4/13/2015 9:16:04 PM
 *  Author: Brandon
 */ 


#ifndef SWITCHINGCIRCUITRY_H_
#define SWITCHINGCIRCUITRY_H_

#define SOCKET_SIZE 40 
#define PIN_TYPES 6
//Array Order
#define LOGIC 0 
#define PULL_DOWN 1
#define GROUND 2 
#define PULL_UP 3 
#define VCC 4
#define VPP 5

  


//Shift Reg Definitions
#define SHIFT_REG_COUNT 5
#define CLEAR 0x00

void SPI_Switching_Circuitry_Init();
void SPI_Switching_Circuitry_Write(unsigned char SPI_Data);
void SwitchingCircuitryEnable();
void ClearShiftRegData();
void SetShiftRegData(uint8_t pinType, char data[SOCKET_SIZE/sizeof(char)]);
void WriteShiftRegData();
void setAtTiny2313();
void setAtMega324PA();


#endif /* SWITCHINGCIRCUITRY_H_ */