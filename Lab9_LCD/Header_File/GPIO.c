//*****************************************************************************
//
// GPIO.c by Allen Hsu.
// First version on Aug. 24. 2014
//
//*****************************************************************************
#include "GPIO.h"

void GPIO_Open(GPIO_GROUP group, unsigned int pin, GPIO_MODE mode)
{
	// IO mode, GP_BA
	volatile unsigned int *reg_ptr;
	
	reg_ptr = (unsigned int *)(GPIO_BASE + group * 0x40); // 0x40 = GPIO_OFFSET
	*reg_ptr |= (unsigned int)(mode << (pin << 2));
}

void GPIO_Output(GPIO_GROUP group, unsigned int pinmask, unsigned int data)
{ 
	volatile unsigned int *reg_ptr;
	
	reg_ptr = (unsigned int *)(GPIO_BASE + group * 0x40 + 0xc); // 0x40 = GPIO_OFFSET
	*reg_ptr = ~pinmask;	
	
	reg_ptr = (unsigned int *)(GPIO_BASE + group * 0x40 + 8); // 0x40 = GPIO_OFFSET
	*reg_ptr = data;
	
}



