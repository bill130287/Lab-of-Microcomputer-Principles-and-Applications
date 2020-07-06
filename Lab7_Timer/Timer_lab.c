#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvTIMER.h"
#include "Driver\DrvGPIO.h"

int RED = 255;
int GREEN = 255;
int BLUE = 255;

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

void TMR0_IRQHandler1(void)
{
	int clk=0;
	
	if(clk<256)
		clk=clk+1;
	else
		clk=0;

	// RED light	
	if(RED<clk)
		DrvGPIO_SetBit(E_GPA,14);
	else
		DrvGPIO_ClrBit(E_GPA,14);
	
	// GREEN light
	if(GREEN<clk)
		DrvGPIO_SetBit(E_GPA,13);
	else 
		DrvGPIO_ClrBit(E_GPA,13);
	
    // BLUE light    
	if(BLUE<clk)
		DrvGPIO_SetBit(E_GPA,12);
	else 
		DrvGPIO_ClrBit(E_GPA,12);
   
	TIMER0->TISR.TIF = 1;
}
	
void TIMER0_Init(void)
{
	// Timer Clock Source
	SYSCLK->CLKSEL1.TMR0_S = 2; // Clock source from HCLK.
	SYSCLK->APBCLK.TMR0_EN = 1;	//Enable Timer0 clock source
	// Setting Mode
	// 00 -> One-shot
	// 01 -> Periodic
	// 10 -> Toggle
	// 11 -> Continuous counting mode
	TIMER0->TCSR.MODE = 1;
	// Setting Prescale
	TIMER0->TCSR.PRESCALE = 0;
	// Data Load Enable
	TIMER0->TCSR.TDR_EN = 1;
	// Counter Mode Enable Bit
	TIMER0->TCSR.CTB = 0;
	// Enable interrupt
	TIMER0->TCSR.IE = 1;	
	// Enable Timer0 Interrupt
	NVIC_EnableIRQ(TMR0_IRQn);	
	//Enable Timer0 Interrupt
	// Disable Timer
	TIMER0->TCSR.CEN = 1;
	// Timer Compared value
	TIMER0->TCMPR = 100;
	
	TIMER0->TCSR.CRST = 1;
	TIMER0->TCSR.CEN = 1;	
}

int main(void)
{
	int i, j;

	int R[7] = {
					0xFF,  // Red
					0xFF, // Orange
					0xFF,  // Yellow
					0x00,  // Green
					0x00,  // Blue
					0x66,  // Deep blue
					0x00   // purple.
				}; // 7 colors 
	int G[7] = {
					0x00,
					0x55,
					0xFF,
		        	0xFF,
					0x00,
					0x00,
					0x00
	           	};
	int B[7] = {
					0x00,
					0x11,
					0x00,
					0x00,
					0xFF,
					0x9D,
					0x00
				};

	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz 
	LOCKREG();
	
	//Choose the RGB pin mode.
	DrvGPIO_Open(E_GPA, 12,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPA, 13,E_IO_OUTPUT);
	DrvGPIO_Open(E_GPA, 14,E_IO_OUTPUT);
	
	// Init Timer.
	TIMER0_Init();

	while(1)
	{	
		for(i=0;i<7;i++)
		{
			RED=R[i];
			GREEN=G[i];
            BLUE=B[i];
			SYS_Delay(500000);
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
