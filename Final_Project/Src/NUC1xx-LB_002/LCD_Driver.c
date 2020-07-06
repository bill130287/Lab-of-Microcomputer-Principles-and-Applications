#include "NUC1xx.h"

#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvGPIO.h"	
#include "NUC1xx-LB_002\LCD_Driver.h"
#define  LCD_Xoffset 129

extern  SPI_T * SPI_PORT[4]={SPI0, SPI1, SPI2, SPI3};

extern char Ascii[];

void SysTimerDelay(uint32_t us)
{
    SysTick->LOAD = us * 22; /* Assume the internal 22MHz RC used */
    SysTick->VAL   =  (0x00);
    SysTick->CTRL = (1 << SysTick_CTRL_CLKSOURCE_Pos) | (1<<SysTick_CTRL_ENABLE_Pos);

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

void Initial_panel(void)
{
	SYSCLK->APBCLK.SPI3_EN  =1;			 //enable spi function 
	SYS->IPRSTC2.SPI3_RST   =1;			 //reset spi function
	SYS->IPRSTC2.SPI3_RST   =0;

	/* set GPIO to SPI mode*/
	SYS->GPDMFP.SPI3_SS0 	=1;
	SYS->GPDMFP.SPI3_CLK 	=1;
	//SYS->GPDMFP.SPI3_MISO0 	=1;
	SYS->GPDMFP.SPI3_MOSI0 	=1;
	// Initial
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.CLKP = 0;							    //CLKP LOW IDLE  
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.TX_BIT_LEN = 9;						//TX LEGTH 9
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.TX_NEG = 1;							  //SET TX_NEG
	SPI_PORT[eDRVSPI_PORT3]->DIVIDER.DIVIDER=3;					  		//SET DIV

	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;									      //ENABLE SLAVE SELECT
	// Set BR[1:0] (Set LCD Bias Ratio)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0xEB;											// 00_1110_1011B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
		
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	// Set BR[1:0] (Set LCD Bias Ratio)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x81;											// 00_1110_1011B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0xA0;											// 00_1010_0000B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
	
		SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0xA1;											// 00_1010_0000B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
	
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	// Set LC[2:1] (Set LCD Mapping Control)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0xc6;											// 00_1100_0000B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
	
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	// Set Display Enable, Set DC[2]
	SPI_PORT[eDRVSPI_PORT3]->TX[0] = 0XAF;										// 00_1010_1111B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
}

//*******************************
//this function to wrtie data to panel
//*******************************
void WriteData(unsigned char data)
{
	// Write Data
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;	   //chip select
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x100 | data;    	//write data
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 ); //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
}

//*******************************
//this function to initial start address
//*******************************
void SetPACA(unsigned char PA, unsigned char CA)
{
		SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	// Set PA[3:0] (Set Page Address)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] = 0xB0 | PA;								 // 1011_0000B | PA
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	 //check data out?
		SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
	
		SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	// Set CA MSB (Set column Address MSB)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x10 |((CA>>4)&0xF);				 // 0001_0000B | (CA>>4) & 1111B
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	  //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
	
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
 	// Set CA LSB (Set column Address LSB)
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x00 | (CA & 0xF);				 // 0000_0000B | (CA & 1111B)		
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	  //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
}

void Disable_Buzzer(void)
{
 	DrvGPIO_Open(E_GPB,11, E_IO_OUTPUT);	
	DrvGPIO_SetBit(E_GPB,11);	
}



void Show_Word(unsigned char x, unsigned char y,unsigned char ascii_word)
{
int i=0,k=0;
	unsigned char temp;	  
	  k=(ascii_word-32)*16;	  
	  
	  for(i=0;i<8;i++)
	  {
		  SetPACA((x*2),(y*8+i+30));
		  temp=Ascii[k+i+8];	 
		  WriteData(temp);
	  }

      for(i=0;i<8;i++)
	  {
		  SetPACA((x*2)+1,(y*8+i+30));	 
		  temp=Ascii[k+i];
		  WriteData(temp);
	  }
}


void print_lcd(unsigned char line, char *str)
{
int i=0; 
  do{   
    Show_Word(line,i,*str++); 
  	i++;
  	if(i>15)
	  break;
    }
   while(*str!='\0');
}



void clr_all_panel(void)
{
 int i=0;
	/*CLEAR ALL PANNEL*/
	SetPACA(0x0, 0x0);			  								  
	for (i = 0; i < 132 *8; i++)
	{
		WriteData(0x00);
	}
	WriteData(0x0f);
}

void draw_LCD(unsigned char *buffer)
{
int Y_max = 8;
int X_max =128;
int y=0;
int x=0;
	
	for (x=0; x< X_max; x++) {
    	for (y=0; y< Y_max; y++) {
			SetPACA(y,(LCD_Xoffset-x));				
			WriteData(buffer[x+y*128]);
			}
		}			
}

void draw_Row(const unsigned char *buffer,int Row)
{
	int x;
	for (x=0; x< 128; x++) {
		SetPACA(Row,(LCD_Xoffset-x));
		WriteData(buffer[x]);
	}
}
