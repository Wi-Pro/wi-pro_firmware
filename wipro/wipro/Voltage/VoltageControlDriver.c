/*
 * VoltageControlDriver.c
 *
 * Created: 3/1/2015 4:26:06 PM
 * Author: Brandon
 * Description: Allows control of the Vpp and Vcc voltage regulators for programming the targeted microcontroller. 
 * A typical use for this library would simply involve calling the setVcc() or setVpp() functions and passing the correct
 * voltage level, as defined in the header file 
 * Example: Set Vcc Regulator to 3.3V		setVcc(VCC_3_3V); 
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "VoltageControlDriver.h"

//Sets output of Vpp Voltage Regulator to 0V
void VppClear()
{
	VEN_PORT &= ~((1<<VPPENABLE));
	VCNTRL_PORT &= ~((1<<VPP_9V) | (1<<VPP_12V) | (1<VPP_13V));
}

void voltageControlInit()
{
	VCNTRL_DDR = 0xFF;
	VCNTRL_PORT = 0x00;
	
	VEN_DDR |= ((1<<VPPENABLE) | (1<<VCCENABLE) | (1<<VLENABLE));
	VEN_PORT &= ~((1<<VPPENABLE) | (1<<VCCENABLE) | (1<<VLENABLE));
}

void enableVppRegulator()
{
	VEN_PORT |= (1<<VPPENABLE); 
}

void disableVppRegulator()
{
	VEN_PORT &= ~((1<<VPPENABLE));
}

//Sets output of Vcc Voltage Regulator to 0V
void VccClear()
{
	VEN_PORT &= ~(1<<VCCENABLE);
	VCNTRL_PORT &= ~((1<<VCC_3_3V) | (1<<VCC_5V) | (1<<VCC_6_5V));
}

void enableVccRegulator() 
{
	VEN_PORT |= (1<<VCCENABLE);
}

void disableVccRegulator()
{
	VEN_PORT &= ~(1<<VCCENABLE);
}

uint8_t setVcc(uint8_t voltageLevel)
{
	//prevents accidental triggering of the wrong regulator 
	//if(voltageLevel != (VCC_3_3V || VCC_5V || VCC_6_5V))
	//	return 0; 
	//else
	//{
		VCNTRL_PORT |= (1<<voltageLevel);
		return 1; 
	//}
}

uint8_t setVpp(uint8_t voltageLevel)
{
	//prevents accidental triggering of the wrong regulator 
	//if(voltageLevel != (VPP_9V || VPP_12V || VPP_13V))
		//return 0; 
	//else
	//{
		VCNTRL_PORT |= (1<<voltageLevel);
		return 1; 
	//}
}

void VLogicClear()
{
	VEN_PORT &= ~(1<<VLENABLE);
	VCNTRL_PORT &= ~((1<<VL_3_3V) | (1<<VL_5V));
}

void enableVLogic()
{
	VEN_PORT |= (1<<VLENABLE);
}

void disableVLogic()
{
	VEN_PORT &= ~(1<<VLENABLE);
}

uint8_t setVLogic(uint8_t voltageLevel)
{
	//prevents accidental triggering of the wrong regulator
	//if(voltageLevel != (VL_3_3V || VL_5V))
	//	return 0;
	//else
	//{
		VCNTRL_PORT |= (1<<voltageLevel);
		return 1;
	//}
}

