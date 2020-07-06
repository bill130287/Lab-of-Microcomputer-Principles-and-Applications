#include <stdio.h>
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include "note_freq.h"

#define  P125ms 125000
#define  P250ms 250000
#define  P500ms 500000
#define  P1S   1000000

void SYS_Delay(unsigned int us)
{
	static unsigned char repeat;
	
	// If sys clock is 25M Hz.
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

void PWM4_Init(void)
{
	PWMB->PPR.CP01 = 99; // Divide by ?
	PWMB->CSR.CSR0 = 0; // Divide by ?
	
	PWMB->PCR.CH0MOD =1 ; // PWM Timer's Mode.
	
	PWMB->CMR0 = 605;
	PWMB->CNR0 = 757;
	
	PWMB->PCR.CH0EN =1 ;  // PWM Timer Starting.
	
	PWMB->POE.PWM0 = 1;
	
}

void PWM_Freq(unsigned int freq)/*unsigned char duty_ratio*/
{
	//duty_ratio=duty_ratio/1000000;
	PWMB->CNR0 =50000000/(200*freq)-1;
	PWMB->CMR0 =(40000000/(200*freq-1))-1;
	
}

int main (void)
{	
	uint16_t music[72] = {
		E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
		C5 ,E5 ,A5 ,B5 ,B5 , 0 ,C5 ,A5 ,B5 ,C6 ,C6 , 0 ,
		E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
		C5 ,E5 ,A5 ,B5 ,B5 , 0 ,E5 ,C6 ,B5 ,A5 ,A5 , 0 ,
		B5 ,C6 ,D6 ,E6 ,E6 , 0 ,G5 ,F6 ,E6 ,D6 ,D6 , 0 ,
		F5 ,E6 ,D6 ,C6 ,C6 , 0 ,E5 ,D6 ,C6 ,B5 ,B5 , 0 
	};
	
	uint32_t pitch[72] = {
		P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,
		P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,
		P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,	P125ms, P125ms, P125ms, P125ms,
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms
	};

	int time;
	UNLOCKREG();
	DrvSYS_Open(50000000);// Set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; //choose STCLK source.
	// Pinmux setting
	SYS->ALTMFP.PB11_PWM4 = 1; // Pinmux
	SYS->GPBMFP.TM3_PWM4 =1 ;  // Pinmux
	SYSCLK->CLKSEL2.PWM45_S =2 ; // PWM
	SYSCLK->APBCLK.PWM45_EN =1 ; // PWM
	LOCKREG();
	
	PWM4_Init();
	
	while(1)
	{
		//Write your code here.
	    for(time=0;time<72;time++)
		{
			 PWM_Freq(music[time]);
			 SYS_Delay(pitch[time]);
		}
	};
}
