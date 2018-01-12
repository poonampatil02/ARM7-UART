#include "adc.c"
#include <LPC214X.H>
void timer0ISR(void)__irq;
unsigned int i;
uart_init()
{
	PINSEL0=(5<<0);
	U0FCR=0X07;
	U0LCR=0X83;
  U0DLL=97;
  U0LCR=0X03;
}
void init_timer0(void)
{
	T0IR=0XFF;
	T0TCR=0X02;
	T0PR=1499;
	T0MCR=0X03;
	T0MR0=40000;
	T0TCR=0X01;
}
void U0write(unsigned int i) //transmit data on uart
{
	while(!(U0LSR & 0X20));
	U0THR=i;
}

unsigned char U0read() //receive data from uart
{
	while(!(U0LSR & 0X01));
	i=U0RBR;
   return i;
}	
	
void U0string (unsigned char *p)
{
	while(*p)
	U0write(*p++);
}	
unsigned short adc_read()
	{
		unsigned int i=0;
		AD0CR=0X00200D00|(1<<3);
		AD0CR |=0X01000000;
		do
		{
			i=AD0GDR;	
		}
		while((i & 0x80000000)==0);
		return((i>>6)&(0x3FF));
	}
void process_adc(void)
	{
		unsigned short adc_value=0;
		unsigned char i;
		adc_value=adc_read();
			U0string (" ADC=");

		lcd_command_write(0x85);
		i=(adc_value/1000)+0x30;
		LCD_data(i);
			U0write(i);
		
		i=((adc_value%1000)/100)+0x30;
		LCD_data(i);
			U0write(i);
		
    i=(((adc_value%1000)%100)/10)+0x30;
		LCD_data(i);
			U0write(i);
		
		i=(adc_value%10)+0x30;
		LCD_data(i);
		
    U0write(i);
		

	}
int main (void) 
{ 
	VPBDIV=0X00;
	PINSEL1=0x10000000;		
	PINSEL2=0x00;	
  IO1DIR= (0x07<<22);	
	IO0DIR=	(0x0F<<10); 
	
	
	//U0string("bicard");
	init_timer0();
	VICIntSelect=0x00;
	VICVectAddr0=(unsigned long int)timer0ISR;
	VICVectCntl0=(0x20)|4;
	VICIntEnable=0x00000010;
	
	init_lcd();
	delay(10);
	lcd_clear();
	lcd_command_write(0x80);
	lcd_write_string("ADC=");
	uart_init();
	
  while(1);
}
void timer0ISR(void)__irq
{
	T0IR=0XFF;
  process_adc(); 
	T0TCR=0X01;
  VICVectAddr=0x00;
	
}


	
