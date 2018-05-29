#include "AD.h"

void AD_Init(void)
{
  PMD1bits.ADCMD  = 0;       //ʹ��ADCģ��
  ADCON0 = 0x00;	//AD�رգ��ұ�־��0
  ADCON1 =0X30;     //ORG:00 �ڲ�Vref 4.096V             
  ADCON2 =0XA0;     //�Ҷ��� 8Tad      
  PIR1bits.ADIF=0;	 //AD�жϱ�־��0       
  PIE1bits.ADIE=0;   //AD�жϹر�     

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
    
	return n;//��AD��ֵ������1000 
}


