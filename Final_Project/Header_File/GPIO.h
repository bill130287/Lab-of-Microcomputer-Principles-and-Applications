//*****************************************************************************
//
// GPIO.h by Allen Hsu.
// First version on Aug. 24. 2014
//
//*****************************************************************************
#ifndef __GPIO_H__
#define __GPIO_H__

#include "Driver\DrvGPIO.h"

// Some define
typedef enum{
	E_GPIOA=0, 
	E_GPIOB,
	E_GPIOC,
	E_GPIOD,
	E_GPIOE
}GPIO_GROUP;

typedef enum{
	E_INPUT=0, 
	E_OUTPUT,
	E_OPEN_DRAIN,
	E_QUASI
}GPIO_MODE;

// API
void GPIO_Open(GPIO_GROUP group, unsigned int pin, GPIO_MODE mode);
void GPIO_Output(GPIO_GROUP group, unsigned int pinmask, unsigned int data);

#endif


