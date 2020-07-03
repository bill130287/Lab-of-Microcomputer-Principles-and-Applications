#include <math.h>
#include "NUC1xx.h"
#include <string.h>
#include <stdlib.h>
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"

void SYS_Delay(unsigned int us)
{
		static unsigned char repeat;
	
		// If sys clock is 25M Hz.
	  repeat = 25;

			SysTick->CTRL &= ~( 1 | 1 << 16 ); 
			SysTick->LOAD = us;
			SysTick->VAL  = 0;
			SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
		while(repeat--){
			/* Waiting for down-count to zero */
			while((SysTick->CTRL & (1 << 16)) == 0);
			SysTick->VAL  = 0;
		}
}

// Range: 0 <= x <= 63
//		  0 <= y <= 127

void Draw_pix( unsigned char  x,  unsigned char  y, unsigned char color)
{ 
	char PA,CA,data,a;
	  
	 
	PA=x/8;
	CA=y;
    a=x%8;
    switch(a)	
	{
		case 0: data=0x01; break;
		case 1: data=0x02; break;
		case 2: data=0x04; break;
		case 3: data=0x08; break;
		case 4: data=0x10; break;
		case 5: data=0x20; break;
		case 6: data=0x40; break;
		case 7: data=0x80; break;
		default: break;
	}
	
	SetPACA( PA, CA);
	WriteData(data);
}

int main(void)
{	
    char x,y,color,tmp;
	int i;
	char number[128];
	UNLOCKREG();
	DrvSYS_Open(50000000); //set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // Select Systemtick Source.
	LOCKREG();
	//panel initial	
	Initial_panel();
	clr_all_panel();
	
	/*
	for(y=0;y<=127;y++)
		{
			 x=32+10*sin((y-1)/20.22);
			 number[y]=x;
			Draw_pix( number[y],  y, color);
	  }
		*/
	tmp=0;
	while(1)
	{ 
	    clr_all_panel();
		for(y=0;y<=127;y++)
		{
			 x=32+10*sin(2*(y-tmp)/20.22);
			 number[y]=x;
			Draw_pix( number[y],  y, color);
	    }
		tmp=tmp+1;
	    SYS_Delay(250000);
		/*
		tmp=number[127];
		for(i=127;i>=0;i--)
		{ 
			
			if(i>0)
			{
				number[i]=number[i-1];
			}
			else
			{
			  number[i]=tmp;
			}
		
		}
		
	for(y=0;y<=127;y++)
		{
			
			Draw_pix( number[y],  y, color);
	  }
		SYS_Delay(150000);
		// Sin Function  y=Vmsin(2pif+x);
	}
	*/
    }
}
void HardFault_Handler(void)
{
	while(1){
		//HardFault
	}
}



