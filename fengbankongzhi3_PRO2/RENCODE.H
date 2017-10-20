/*
���ܣ��t�⾀ң��������
ʹ����Դ��һ��IO���жϣ�һ����ʱ������ʱ��ʱ��ΪACLK,��ʱ��Ƶ��Ϊ32768HZ,Ҫȷ��XT1����
*/
#ifndef _USE0CODE_
#define _USE0CODE_
#include<msp430f5529.h>
#include<intrinsics.h>

#define uchar unsigned char
#define uint  unsigned int
#define INIFG  BIT0
extern unsigned char mode;
uchar I_count=0;
uchar start=0;
long KEY_value=0;
uint temp_time=0;
uint key_time=0;
uchar USRE0=0;
uchar USRE1=0;
uchar key=0xff;
uchar buff[]="00";
uchar function_key;
void key_decoding(uchar key_1);
void inter_init();
void TA0_init();

void inter_init()
{     
      P2REN|=INIFG;///������ƣ�
      P2OUT|=INIFG;///����������
      P2DIR&=~INIFG;
      P2IES|=INIFG;
      P2IE|=INIFG;
      P1DIR|=BIT0;
      
}


//for hongwai
void TA0_init()
{
    TA0CTL|=TASSEL_1+TACLR+TAIE;
    TA0CCR0=2048; 
}

#pragma vector=PORT2_VECTOR
__interrupt void cood_interrup()
{     
        P1OUT^=BIT0;  //ָʾ������
        if(P2IFG&INIFG)
        {
                P2IFG&=~INIFG;
                I_count++;
		if(start)///�������жϵ���34���ж���װ���ݹ�����
		{
                 temp_time=TA0R;
		/*temp_time=TH1;
		temp_time<<=8;
		temp_time+=TL1;*/
			if(temp_time>49)///����λΪ1��
			KEY_value|=0x01;
			KEY_value<<=1;
			if(I_count==34)
			{
				KEY_value>>=1;///	
				USRE1=KEY_value>>24;
				USRE0=KEY_value>>16;
				if((USRE1^USRE0)==0XFF)
					key=KEY_value>>8;
			I_count=0;KEY_value=0; start=0;
			TA0CTL&=~MC0;//TR1=0;
			}
			TA0R=0;//TH1=0;TL1=0;///ÿһ�μ�ⶨʱ���������㣻
		
		
		}
		else 
		{
				if(I_count==1)
                                //TACTL&=~MC0;  //����Ҳ�����˺�����//��������
				TA0CTL|=MC0;
                                
				else if(I_count==2)
				{
					  temp_time=TA0R;
					  /*temp_time=TH1;
					  temp_time<<=8;
					  temp_time+=TL1;*/
					  if(temp_time>393)///�޸Ĵ˴���
					  start=1;
					  TA0R=0;KEY_value=0;//TH1=0;TL1=0;
				}
				else {I_count=0;TA0CTL&=~MC0;TA0R=0;KEY_value=0;}//TH1=0;TL1=0;���û�еڶ����ж�˵���Ǹ��ţ�
                          
		}
        }
}
///��ʱʱ�䵽�����㣻
#pragma vector=TIMER0_A1_VECTOR
__interrupt void time0()
{
      switch(TA0IV)
      {
      case 2:break;
      case 4:break;
      case 14:I_count=0;TA0CTL&=~MC0;TA0R=0;start=0;break;///���㺯��TR1=0;TH1=0;TL1=0;
      }

}
void key_decoding(uchar key_1)	///ת��Ϊ16��������
{
		
        switch(key_1)
        {
          case 0x22: function_key='X';break;
          case 0x68: function_key='0';break;
          case 0x30: function_key='1';break;
          case 0x18: function_key='2';break;
          case 0x7a: function_key='3';break;
          case 0x10: function_key='4';break;
          case 0x38: function_key='5';break;
          case 0x5a: function_key='6';break;
          case 0x42: function_key='7';break;
          case 0x4a: function_key='8';break;
          case 0x52: function_key='9';break;
          case 0xC2: function_key='E';break;
          case 0xa8: function_key='+';break;
          case 0xe0: function_key='-';break;
          case 0xe2: function_key='F';break;
          case 0xA2: {function_key='M';mode++; if(mode==3) mode=0;  }break;
        default :break;
        }
}

#endif 