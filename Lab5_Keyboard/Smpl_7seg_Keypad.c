//
// Smpl_7seg_keypad
//
// Input:  3x3 keypad (input = 1~9 when key is pressed, =0 when key is not pressed
// Output: 7-segment LEDs
//
#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"
#define SEG_0   0x82 
#define SEG_1	0xEE
#define SEG_2	0x07
#define SEG_3	0x46
#define SEG_4 	0x6A
#define SEG_5	0x52	
#define SEG_6	0x1A
#define SEG_7	0xE6	
#define SEG_8	0x02
#define SEG_9   0x62  



unsigned int SEG[10]={SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9}; 
volatile unsigned int *reg_ptr;

unsigned char seg[4] = {0x0, 0x0, 0x0, 0x0};

void SYS_Delay(unsigned int us)
{
    static unsigned char repeat;
	
	// If STCLK is 25M Hz.
	repeat = 25;
	SysTick->CTRL &= ~( 1 | 1 << 16 ); 
	SysTick->LOAD = us;
	SysTick->VAL  = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	while(repeat--)
    {
	    /* Waiting for down-count to zero */
		while((SysTick->CTRL & (1 << 16)) == 0);
		SysTick->VAL  = 0;
	}	
}

void GPC_set(unsigned int group, unsigned int pin)
{
    reg_ptr = (unsigned int *)(0x50004000 + group * 0x40 + 0x08);
    *reg_ptr = (unsigned int)((1<<pin) | (0xf<<12));
}

void show_seven_segment(unsigned int place, unsigned int number)
{
    unsigned int temp,i;
    temp=SEG[number];
	
    for(i=0;i<8;i++)
    {
	    if((temp&0x01)==0x01)
        {		   	  
		    DrvGPIO_SetBit(E_GPE,i);
	    }
	    else
        {
		    DrvGPIO_ClrBit(E_GPE,i);	
	    }	  
		temp=temp>>1;
    }
	GPC_set(E_GPC,3+place);	
}

void OpenKey_Pad(void)
{
	uint8_t i;
	/* Initial key pad mode */
	for(i=0;i<6;i++)
	    DrvGPIO_Open(E_GPA, i, E_IO_QUASI);
}

uint8_t Scan_key(void)
{
	uint8_t act[3]={0x3, 0x5, 0x6};    
	uint8_t i,temp,pin;

	for(i=0;i<3;i++)
	{
		temp=act[i];
		for(pin=0;pin<3;pin++)
		{
		    if((temp&0x01)==0x01)
				DrvGPIO_SetBit(E_GPA,pin);
			else
				DrvGPIO_ClrBit(E_GPA,pin);
			temp>>=1;		
		}
		if(DrvGPIO_GetBit(E_GPA,3)==0)
			return(i+1);
		if(DrvGPIO_GetBit(E_GPA,4)==0)
		    return(i+4);
		if(DrvGPIO_GetBit(E_GPA,5)==0)
			return(i+7);
	}
	return 0;
}

int32_t main (void)
{	
	int key,temp;
	
	UNLOCKREG();
    DrvSYS_Open(48000000);
	LOCKREG();

	OpenKey_Pad();					 	
	
	while(1)
	{				
	    key = Scan_key();
		temp = key;	

		if(temp != 0)
		{
		    seg[3]=seg[2];
			seg[2]=seg[1];
			seg[1]=seg[0];
			seg[0]=temp;	
		}
		while(key == temp)
		{
		    key = Scan_key();
			//show_seven_segment();		
			show_seven_segment(1, seg[0]);
			SYS_Delay(2000);
       	    show_seven_segment(2, seg[1]);
			SYS_Delay(2000);
			show_seven_segment(3, seg[2]);
			SYS_Delay(2000);
			show_seven_segment(4, seg[3]);
			SYS_Delay(2000);	
		}
			
	}	
}

void HardFault_Handler(void)
{
	while(1)
    {
		//HardFault
	}
}

