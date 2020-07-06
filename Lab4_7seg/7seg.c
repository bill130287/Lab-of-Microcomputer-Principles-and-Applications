#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"

//define 7seg codex here
#define SEG_0   0x82 
#define SEG_1   0xEE
#define SEG_2   0x07
#define SEG_3   0x46
#define SEG_4   0x6A
#define SEG_5   0x52	
#define SEG_6   0x1A
#define SEG_7   0xE6	
#define SEG_8   0x02
#define SEG_9   0x62  

#define groupC  2
#define groupE  4

unsigned int SEG[10]={SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9}; 

volatile unsigned int *reg_ptr;
// display an integer on four 7-segment LEDs

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

int32_t main (void)
{
	unsigned int set_number;
	
	unsigned int second1=0,second2=0,minute1=0,minute2=0;
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz. 
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // STCLK Clock Source.
	LOCKREG();	
	
	
	
	
	while(1)
	{
		for(set_number=0;set_number<15;set_number++)
		{
			/*{
			show_seven_segment(set_number,second1);
			}*/
			show_seven_segment(1,second1);
			SYS_Delay(5000);
			show_seven_segment(2,second2);
			SYS_Delay(5000);
			show_seven_segment(3,minute1);
			SYS_Delay(5000);
			show_seven_segment(4,minute2);
			SYS_Delay(5000);
		}
		second1++;
		if(second1==10)
		{
			second1=0;
			second2++;
		    if(second2==6)
			{
				second2=0;
				second1=0;
				minute1++;
				if(minute1==10)
				{
					minute1=0;
					second1=0;
					second2=0;
					minute2++;
					if(minute2==6)
					{
						minute2=0;
						minute1=0;
						second1=0;
						second2=0;
					}
				}		
			}
		}
		/*
		else if(second2==6&&second1==10)
		{
			second2=0;
			second1=0;
			minute1++;
		}
		else if(minute1==9&&second2==6&&second1==10)
		{
			minute1=0;
			second1=0;
			second2=0;
			minute2++;
		}
		else if(minute2==6&&minute1==10&&second2==6&&second2==0)
		{
			minute2=0;
			minute1=0;
			second1=0;
			second2=0;
		}
		*/
	}
}
