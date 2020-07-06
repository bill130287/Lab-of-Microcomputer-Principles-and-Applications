#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"
#define groupC 0x0080
#define groupD 0x00C0
#define groupE 0x0100

#define MODE_input 0
#define MODE_output 1
#define MODE_OpenDrain 2
#define MODE_Quasibidirectional 3

int array_scandata [2][11]={{0x0F,0x07,0x0B,0x0D,0x0E,0x06,0x0A,0x0C,0x04,0x08,0x00},
			    {0x0F,0x0E,0x0D,0x0B,0x07,0x06,0x05,0x03,0x02,0x01,0x00}};
void SYS_Delay(unsigned int us)
{
	static unsigned char repeat;
	
	// If STCLK is 25M Hz.
	//
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

void GPIO_Mode_Select(int group,int pin,int mode)
{
    	volatile unsigned int*reg_ptr,reg_data;
	reg_ptr=(unsigned int*)(0x50004000+group);
	reg_data=*reg_ptr&(~(3<<(pin<<1)));
    	*reg_ptr=reg_data|(mode<<(pin<<1));

	return;
}

void GPIO_Write(int group,int pin,int data)
{
	volatile unsigned int*reg_ptr,reg_data;
	reg_ptr=(unsigned int*)(0x50004000+group+0x8);
	reg_data=*reg_ptr&(~(1<<pin));
	*reg_ptr=reg_data|(data<<pin);
	
	return;
}
void ledout (unsigned int scandata)
{
	int scan,data;
	
	for(scan=15; scan>11; scan--)
	{
		data = (0x01)&(scandata >> (15-scan));
		GPIO_Mode_Select (groupC, scan, MODE_output);
		GPIO_Write(groupC,scan,data);
	}
}

int main(void)
{	
    	int num,data;
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz. 
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // STCLK Clock Source.
	LOCKREG();
	
	GPIO_Mode_Select(groupC,12,MODE_output);
	GPIO_Mode_Select(groupC,13,MODE_output);
	GPIO_Mode_Select(groupC,14,MODE_output);
	GPIO_Mode_Select(groupC,15,MODE_output);	
 
	while(1)
    	{
		for(num=0;num<2;num++)
		{
			for(data=0;data<11;data++)
		    	{
				ledout(array_scandata[num][data]);
				SYS_Delay(1000000);
			}
		}
		 
		/*
		ledout(0x07);
		SYS_Delay(100000);
		ledout(0x0B);
		SYS_Delay(100000);
		ledout(0x0D);
		SYS_Delay(100000);
	    	ledout(0x0E);
		SYS_Delay(100000);
		ledout(0x0E);
		SYS_Delay(100000);
		ledout(0x0D);
		SYS_Delay(100000);
		ledout(0x0B);
		SYS_Delay(100000);
		ledout(0x07);
		SYS_Delay(100000); 
	    	/*
	    	for(time=0;time<4;time++)
	    	{
			ledout(scandata);
		    	scandata=scandata >> 1;
	    	}
		
		GPIO_Write(groupC,12,0);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,0);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,0);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,0);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,0);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,0);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,1);
		GPIO_Write(groupC,13,0);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		GPIO_Write(groupC,12,0);
		GPIO_Write(groupC,13,1);
		GPIO_Write(groupC,14,1);
		GPIO_Write(groupC,15,1);
		SYS_Delay(1000000);
		*/
    }
}


void HardFault_Handler(void)
{
	while(1)
    	{
		//HardFault
	}
}
