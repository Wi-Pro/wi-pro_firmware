/*
 * VoltageControlDriver.h
 *
 * Created: 3/1/2015 4:42:19 PM
 *  Author: Brandon
 */ 


#ifndef VOLTAGECONTROLDRIVER_H_
#define VOLTAGECONTROLDRIVER_H_

#define VCNTRL_PORT PORTF
#define VCNTRL_DDR DDRF
#define VEN_PORT PORTB 
#define VEN_DDR DDRB  

#define VPPENABLE 4
#define VPP_9V 2
#define VPP_12V 1
#define VPP_13V 0

#define VCCENABLE 5
#define VCC_3_3V 5
#define VCC_5V 4
#define VCC_6_5V 3

#define VLENABLE 6
#define VL_3_3V 7
#define VL_5V 6

void voltageControlInit();
void VppClear();
void enableVppRegulator();
void disableVppRegulator();
void VccClear();
void enableVccRegulator(); 
void disableVccRegulator();
void VLogicClear(); 
void enableVLogic(); 
void disableVLogic(); 
uint8_t setVcc(uint8_t voltageLevel);
uint8_t setVpp(uint8_t voltageLevel);
uint8_t setVLogic(uint8_t voltageLevel);


#endif /* VOLTAGECONTROLDRIVER_H_ */