#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\Seven_Segment.h"
#define SEG_N0   0x82 
#define SEG_N1   0xEE 
#define SEG_N2   0x07 
#define SEG_N3   0x46 
#define SEG_N4   0x6A  
#define SEG_N5   0x52 
#define SEG_N6   0x12 
#define SEG_N7   0xE6 
#define SEG_N8   0x02 
#define SEG_N9   0x62
#define SEG_N10	 0x22
#define SEG_N11  0x1A
#define SEG_N12  0x93
#define SEG_N13  0x0E
#define SEG_N14  0x13
#define SEG_N15  0x33

unsigned char SEG_BUF[16]={SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_N10, SEG_N11, SEG_N12, SEG_N13, SEG_N14, SEG_N15}; 
volatile unsigned int *reg_ptr; 

void GPC_set(unsigned int pin)
{
reg_ptr = (unsigned int *)(0x50004000 + 0x8C);
*reg_ptr = (unsigned int)(0xf<<12);
reg_ptr = (unsigned int *)(0x50004000 + 0x88);
*reg_ptr = (unsigned int)(1<<pin);
}
void show_seven_segment(unsigned char place, unsigned char number)
{
  unsigned char temp,i;
	temp=SEG_BUF[number];
	
	for(i=0;i<8;i++)
	    {
		if((temp&0x01)==0x01)		   		   
		   DrvGPIO_SetBit(E_GPE,i);
		   else
		   DrvGPIO_ClrBit(E_GPE,i);		  
		   temp=temp>>1;
		}
		GPC_set(4+place);	
}
