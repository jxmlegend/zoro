#include "AD.h"

void AD_Init(void)
{
  PMD1bits.ADCMD  = 0;       //使能ADC模块
  ADCON0 = 0x00;	//AD关闭，且标志置0
  ADCON1 =0X30;     //ORG:00 内部Vref 4.096V             
  ADCON2 =0XA0;     //右对齐 8Tad      
  PIR1bits.ADIF=0;	 //AD中断标志置0       
  PIE1bits.ADIE=0;   //AD中断关闭     

  TRISA = 0xff;
  PORTA = 0x00;
//  ANCON0=0xff;
//  ANCON1=0x00;     
}
INT16U AD_GET(INT8U channel)
{
	INT16U n;
	ADCON0 = channel;
	ADCON0bits.ADON = 1;
//	for(n=0;n<50;n++)
//	{
//        Nop();	
//	}
	Delay10TCYx(5);
	ADCON0bits.GO = 1;
	while(ADCON0bits.GO);
	n = ADRESH*256+ ADRESL; 
	ADCON0bits.ADON = 0;
    
	return n;//把AD的值扩大了1000 
}


