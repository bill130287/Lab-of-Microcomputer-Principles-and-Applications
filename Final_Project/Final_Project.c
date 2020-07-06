#include <math.h>
#include <time.h>
#include "NUC1xx.h"
#include <string.h>
#include <stdlib.h>
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include "note_freq.h"

#define groupC 0x0080
#define MODE_output 1

//define 7seg codex here
#define SEG_0   0x82 
#define SEG_1		0xEE
#define SEG_2		0x07
#define SEG_3		0x46
#define SEG_4 	0x6A
#define SEG_5		0x52	
#define SEG_6		0x1A
#define SEG_7		0xE6	
#define SEG_8		0x02
#define SEG_9   0x62  

unsigned int SEG[10]={SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9}; 




volatile unsigned int *reg_ptr;

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
//				0 <= y <= 127
void Draw_pix_y( unsigned char  x,  unsigned char  y, unsigned char color)
{ 
	  char  PA,CA,data,a;
	  
	  
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

void GPC_set(unsigned int group, unsigned int pin)
{
reg_ptr = (unsigned int *)(0x50004000 + group * 0x40 + 0x08);
*reg_ptr = (unsigned int)((1<<pin) | (0xf<<12));
}

void show_seven_segment(unsigned int place, unsigned int number)
{
unsigned int temp,i;
temp=SEG[number];
	
for(i=0;i<8;i++){
	 if((temp&0x01)==0x01){		   	  
		DrvGPIO_SetBit(E_GPE,i);
	 }
	else{
		DrvGPIO_ClrBit(E_GPE,i);	
	}	  
		temp=temp>>1;
}
	GPC_set(E_GPC,3+place);	
}

void PWM4_Init(void)
{
	PWMB->PPR.CP01 = 99; // Divide by ?
	PWMB->CSR.CSR0 = 0; // Divide by ?
	
	PWMB->PCR.CH0MOD =0 ; // PWM Timer's Mode.
	
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



void enemy_plane(unsigned char  a,  unsigned char  b,unsigned char  page)
{
	  char Y;
	  char enemy_data[21]={0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xCC,0xCC,0xCC,0xFF,0xFF,0xFF,0xCC,0xCC,0xCC,0xCC,0x0C,0x0C,0x0C,0x0C,0x0C};
		
	 for(Y=a;Y<b;Y++)
		{
		  SetPACA( page, Y);
	    WriteData(enemy_data[Y-a]);
		}
}

void shoot_in_one_page(unsigned char  a,  unsigned char  b, unsigned char  c)
{
	char bullet=0x01;
	 SetPACA( c, (a+b)/2-2);
	       WriteData(bullet*pow(2,c));
				 
				 SetPACA( c, (a+b)/2-1);
	       WriteData(bullet*pow(2,c));
				 
				 SetPACA( c, (a+b)/2);
	       WriteData(bullet*pow(2,c));
				 SYS_Delay(70000);
}
/*
char bomb(unsigned char  a,  unsigned char  b,unsigned char  d,unsigned char  page_enemyplane1,unsigned char  page_enemyplane2,unsigned char  page_enemyplane3)
{
	 char Y,tmp;
		tmp=(a+b)/2;
	if((tmp>=30&&tmp<=53)&&d==page_enemyplane1)
	{
		for(Y=30;Y<53;Y++)
		{
		  SetPACA( page_enemyplane1, Y);
	    WriteData(0);
		}
		
		
		
		  
	}
			
		
	
	if ((tmp>=54&&tmp<=77)&&d==page_enemyplane2)
	{
		for(Y=54;Y<77;Y++)
		{
		  SetPACA( page_enemyplane2, Y);
	    WriteData(0);
		}
		return(1);
				 SetPACA( d, (a+b)/2-2);
	       WriteData(0);
				 
				 SetPACA( d, (a+b)/2-1);
	       WriteData(0);
				 
				 SetPACA( d, (a+b)/2);
	       WriteData(0);
				 SYS_Delay(70000/2);
  }		
  if ((tmp>=78&&tmp<=101)&&d==page_enemyplane3)
	{
		for(Y=78;Y<101;Y++)
		{
		  SetPACA( page_enemyplane3, Y);
	    WriteData(0);
		}
		 SetPACA( d, (a+b)/2-2);
	       WriteData(0);
				 
				 SetPACA( d, (a+b)/2-1);
	       WriteData(0);
				 
				 SetPACA( d, (a+b)/2);
	       WriteData(0);
				 SYS_Delay(70000/2);*
	}
 }*/


int main(void)
{	
  char x,y,Y,color,tmp1,key,temp,bullet,ready,go,pause,start;
	signed int speed,move1;
	signed char data_bu,page_bu,count,truth,bu_tmp;
	char second1,second2,minute1,minute2,time;
	int i,a,b,a1,b1;
	signed char life,page1,page2,page3,page4,page5,page6,page7,page8,page9,page10,page11,set_number;
	signed char plane1,plane2,plane3,plane4,plane5,plane6,plane7,plane8,plane9,plane10,plane11;
	signed char cout_c,cout_d,array_number;
	char       data[21]={0x30,0x30,0x30,0x30,0x30,0x33,0x33,0x33,0x33,0xFF,0xFF,0xFF,0x33,0x33,0x33,0x33,0x30,0x30,0x30,0x30,0x30};
	char life_array[4]={0x01,0x03,0x07,0x0F};
//	char plane[5]={plane1,plane2,plane3,plane4,plane5};
	UNLOCKREG();
	DrvSYS_Open(50000000); //set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // Select Systemtick Source.
	

	SYS->ALTMFP.PB11_PWM4 = 1; // Pinmux
	SYS->GPBMFP.TM3_PWM4 =1 ;  // Pinmux
	SYSCLK->CLKSEL2.PWM45_S =2 ; // PWM
	SYSCLK->APBCLK.PWM45_EN =1 ; // PWM
	LOCKREG();
	//panel initial	
	PWM4_Init();
	Initial_panel();
	clr_all_panel();
	
	GPIO_Mode_Select(groupC,12,MODE_output);
	GPIO_Mode_Select(groupC,13,MODE_output);
	GPIO_Mode_Select(groupC,14,MODE_output);
	GPIO_Mode_Select(groupC,15,MODE_output);
		
		
		
	a=54;
	b=76;
	a1=54;
	b1=76;
	data_bu=0; page_bu=1;
	bullet=0x01;
	life=0x00;
	
	move1=24;
	
	
	page1=5;
	page2=5;
	page3=7;
	page4=7;
	page5=7;
	page6=9;
	page7=9;
	page8=9;
	page9=11;
	page10=11;
	page11=11;
	
	plane1=6;
	plane2=30;
	plane3=54;
	plane4=78;
	plane5=102;
	plane6=6;
	plane7=54;
	plane8=78;
	plane9=6;
	plane10=30;
	plane11=102;
	
	go=0;
	ready=0;
	time=0;
	count=0;
	array_number=0;
	speed=20000;
	while(1)
	{ 
		ledout(life);
		//plane enemy
		
		 while(ready<3)
		{
			print_lcd(2,"  Ready");
			ready+=1;
			SYS_Delay(750000);
		};
		while(go<2)
		{
			
			print_lcd(1,"   Go");
			go+=1;
			SYS_Delay(800000);
		}
		pause=Scan_key();
		while(pause==5)
		{
			 print_lcd(2,"  Pause");
			 start=Scan_key();
			
			show_seven_segment(1,second1);
			SYS_Delay(750);
			show_seven_segment(2,second2);
			SYS_Delay(750);
			show_seven_segment(3,minute1);
			SYS_Delay(750);
			show_seven_segment(4,minute2);
			SYS_Delay(750);
			ledout(life);
			 if(start==2)
			{
				break;
			}
		}
		
		 if(count==20)
		{
			page1--;
			page2--;
			page3--;
			page4--;
			page5--;
			page6--;
	    page7--;
	    page8--;
			page9--;
	    page10--;
	    page11--;
	    
			enemy_plane(plane1 ,plane1+22,page1);
			enemy_plane(plane4 ,plane4+22,page2);
		  enemy_plane(plane2 ,plane1+22,page3);
	    enemy_plane(plane3 ,plane3+22,page4);
			enemy_plane(plane5 ,plane5+22,page5);
			enemy_plane(plane6 ,plane6+22,page6);
			enemy_plane(plane7 ,plane7+22,page7);
		  enemy_plane(plane8 ,plane8+22,page8);
	    enemy_plane(plane9 ,plane9+22,page9);
		  enemy_plane(plane10 ,plane10+22,page10);
			enemy_plane(plane11 ,plane11+22,page11);
		  
			count=0;
			if(page1<0)
		 {  
				page1=7;
			 plane1+=move1;
				if(plane1>102)
			{
				plane1=6;
			}
		 }
		 	if(page2<0)
		 {
				page2=7;
			 plane4+=move1;
				if(plane4>102)
			{
				plane4=6;
			}
		 }
		 	if(page3<0)
		 {
				page3=7;
			 plane2+=move1;
				if(plane2>102)
			{
				plane2=6;
			}
		 }
		 if(page4<0)
		 {
				page4=7;
			 plane3+=move1;
				if(plane3>102)
			{
				plane3=6;
			}
		 }
		 	if(page5<0)
		 {
				page5=7;
			 plane5+=move1;
				if(plane5>102)
			{
				plane5=6;
			}
		 }
		 	if(page6<0)
		 {
				page6=7;
			 plane6+=move1;
				if(plane6>102)
			{
				plane6=6;
			}
		 }
		 if(page7<0)
		 {
				page7=7;
			 plane7+=move1;
				if(plane7>102)
			{
				plane7=6;
			}
		 }
		 	if(page8<0)
		 {
				page8=7;
			 plane8+=move1;
				if(plane8>102)
			{
				plane8=6;
			}
		 }
		 if(page9<0)
		 {
				page9=7;
			 plane9+=move1;
				if(plane9>102)
			{
				plane9=6;
			}
		 }
		 if(page10<0)
		 {
				page10=7;
			 plane10+=move1;
				if(plane10>102)
			{
				plane10=6;
			}
		 }
		 if(page11<0)
		 {
				page11=7;
			 plane11+=move1;
				if(plane11>102)
			{
				plane11=6;
			}
		 }
		}
		else
		{	
			enemy_plane(plane1 ,plane1+22,page1);
			enemy_plane(plane4 ,plane4+22,page2);
		  enemy_plane(plane2 ,plane2+22,page3);
	    enemy_plane(plane3 ,plane3+22,page4);
			enemy_plane(plane5 ,plane5+22,page5);
			enemy_plane(plane6 ,plane6+22,page6);
			enemy_plane(plane7 ,plane7+22,page7);
		  enemy_plane(plane8 ,plane8+22,page8);
	    enemy_plane(plane9 ,plane9+22,page9);
		  enemy_plane(plane10 ,plane10+22,page10);
			enemy_plane(plane11 ,plane11+22,page11);
	   
		}
		//plane_bomb
		if((a>=plane1&&b<=(plane1+22))&&page1==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page1=8;
			PWM_Freq(1000);
			plane1+=move1;
				if(plane1>102)
			{
				plane1=6;
			}
			
				
			
			
		}	
		
		if((a>=plane4&&b<=(plane4+22))&&page2==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page2=8;
			PWM_Freq(1000);
			plane4+=move1;
			   if(plane4>102)
			{
				plane4=6;
			}
			
				
			
		}
		if((a>=plane2&&b<=(plane2+22))&&page3==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page3=8;
			PWM_Freq(1000);
			plane2+=move1;
				if(plane2>102)
			{
				plane2=6;
			}
			
				
			
		}
		if((a>=plane3&&b<=(plane3+22))&&page4==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page4=8;
			PWM_Freq(1000);
			plane3+=move1;
				if(plane3>102)
			{
				plane3=6;
			}
			 
				
			
			
		}	
		if((a>=plane5&&b<=(plane5+22))&&page5==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page5=8;
			PWM_Freq(1000);
			plane5+=move1;
				if(plane5>102)
			{
				plane5=6;
			}
			
				
			
			
		}	
		if((a>=plane6&&b<=(plane6+22))&&page6==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page6=8;
			PWM_Freq(1000);
			plane6+=move1;
				if(plane6>102)
			{
				plane6=6;
			}
			
				
			
			
		}	
		if((a>=plane7&&b<=(plane7+22))&&page7==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page7=8;
			PWM_Freq(1000);
			plane7+=move1;
				if(plane7>102)
			{
				plane7=6;
			}
			 
				
			
			
		}	
		if((a>=plane8&&b<=(plane8+22))&&page8==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page8=8;
			PWM_Freq(1000);
			plane8+=move1;
				if(plane8>102)
			{
				plane8=6;
			}
			
				
			
			
		}	
		
		
		
		if((a>=plane9&&b<=(plane9+22))&&page9==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page9=8;
			PWM_Freq(1000);
				plane9+=move1;
		  	if(plane9>102)
			{
				plane9=6;
			}
			
			
			
			
		}	
		
		
		
		
		if((a>=plane10&&b<=(plane10+22))&&page10==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page10=8;
			PWM_Freq(1000);
			plane10+=move1;
				if(plane10>102)
			{
				plane10=6;
			}
			
				
			
			
		}	
		if((a>=plane11&&b<=(plane11+22))&&page11==0)
		{
			life=life_array[array_number];
			array_number+=1;
			page11=8;
			PWM_Freq(1000);
			SYS_Delay(20000);
			plane11+=move1;
				if(plane11>102)
			{
				plane11=6;
			}
			 
				
			
		}	
		
		while(array_number>=4)
	{ 
		Initial_panel();
		clr_all_panel();
		 
	  print_lcd(2, "Game over");
	  
	   
		show_seven_segment(1,second1);
			SYS_Delay(5000);
			show_seven_segment(2,second2);
			SYS_Delay(5000);
			show_seven_segment(3,minute1);
			SYS_Delay(5000);
			show_seven_segment(4,minute2);
			SYS_Delay(5000);
		
	}
		
		// plane own
		for(y=a;y<b;y++)
		{
		  SetPACA( 0, y);
	    WriteData(data[y-a]);
		}
		
    //shift left
		
		if(Scan_key()==4)
		{ 
			
						
			
			 a-=24;
			 b-=24;
					
			key = Scan_key();
			temp = key;	

		  if(temp != 0)
			 {
			    if(a<6)
			   {
				  a=6;
				  b=28;
			   }			
		   }
			
			while(key == temp)
			{
				key = Scan_key();
		  }
			
		}	 
			//clr_all_panel();
		
		//shift right
		else if(Scan_key()==6)
		{
			 a+=24;
			 b+=24;
			key = Scan_key();
			temp = key;	

		  if(temp != 0)
			 {
					if(b>124)
					{
						a=102;
						b=124;
					}
		   }
			while(key == temp)
			{
				key = Scan_key();
		  } 
				 
		 }
			
			
			//clr_all_panel();
		
		
		//score
		
  
			show_seven_segment(1,second1);
			SYS_Delay(750);
			show_seven_segment(2,second2);
			SYS_Delay(750);
			show_seven_segment(3,minute1);
			SYS_Delay(750);
			show_seven_segment(4,minute2);
			SYS_Delay(750);
	    if(time==25)
	  {
		 second1++;
			time=0;
		}
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
	
	  	/*if(truth==1)
		{  //score
			SYS_Delay(15000);
			score1+=1;
	    truth=0;
			if(score1==10)
		{
			score1=0;
			score2++;
			  if(score2==10)
			{
				score2=0;
				score1=0;
				score3++;
						if(score3==10)
					{
						score3=0;
						score1=0;
						score2=0;
						score4++;
								if(score4==10)
							{
								score4=0;
								score3=0;
								score2=0;
								score1=0;
							}
					}
						
			}
		}
	}
	   
		
	
	    show_seven_segment(1,score1);
	    SYS_Delay(2000);
		  show_seven_segment(2,score2);
	    SYS_Delay(2000);
		  show_seven_segment(3,score3);
	    SYS_Delay(2000);
		  show_seven_segment(4,score4);
	    SYS_Delay(2000);
		*/
	 
	
	
	
	 
	  time++;
	 count++;
	
	 if(speed<=10)
	 {
		 speed=10;
	 }
	 else
	 {
		  speed-=10;
	 }
	 SYS_Delay(speed);
		clr_all_panel();

		
		
	
	
 }
}

	 
void HardFault_Handler(void)
{
	while(1){
		//HardFault
	}
}









