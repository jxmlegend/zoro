/*
功能：紅外線遥控器解码
使用资源：一个IO口中断，一个定时器（定时器时钟为ACLK,且时钟频率为32768HZ,要确保XT1起振）
*/
#ifndef _USE0CODE_
#define _USE0CODE_
#include<msp430f5529.h>
#include<intrinsics.h>

#define uchar unsigned char
#define uint  unsigned int
#define INIFG  BIT5
uchar I_count=0;
uchar start=0;
long KEY_value=0;
uint temp_time=0;
uint key_time=0;
uchar USRE0=0;
uchar USRE1=0;
uchar key=0xff;
uchar buff[]="00";

extern unsigned int num;
extern unsigned long int dis;
extern unsigned char buff_dis[14];
unsigned char dis_flag;
void key_decoding(uchar key_1);
void inter_init();
void TA1_init();

void inter_init()
{     
      P2REN|=INIFG;///电阻控制；
      P2OUT|=INIFG;///电阻上拉；
      P2DIR&=~INIFG;
      P2IES|=INIFG;
      P2IE|=INIFG;   
}


//for hongwai
void TA1_init()
{
    TA1CTL|=TASSEL_1+TACLR+TAIE;
    TA1CCR0=2048; 
}

#pragma vector=PORT2_VECTOR
__interrupt void cood_interrup()
{     
        if(P2IFG&BIT0)
        {
          P2IFG&=~BIT0; 
          num++;  
          dis=num*6;
          dis_flag=1;          
        }
        if(P2IFG&INIFG) 
        {
                P2IFG&=~INIFG;
                I_count++;
		if(start)///第三次中断到第34次中断做装数据工作；
		{
                 temp_time=TA1R;
		/*temp_time=TH1;
		temp_time<<=8;
		temp_time+=TL1;*/
			if(temp_time>49)///数据位为1；
			KEY_value|=0x01;
			KEY_value<<=1;
			if(I_count==34)
			{
				KEY_value>>=1;	
				USRE1=KEY_value>>24;
				USRE0=KEY_value>>16;
				if((USRE1^USRE0)==0XFF)
					key=KEY_value>>8;
			I_count=0;KEY_value=0; start=0;
			TA1CTL&=~MC0;//TR1=0;
			}
			TA1R=0;//TH1=0;TL1=0;///每一次检测定时器必须清零；
		
		
		}
		else 
		{
				if(I_count==1)
                                //TACTL&=~MC0;  //连码也会进入此函数；//但第三次
				TA1CTL|=MC0;
                                
				else if(I_count==2)
				{
					  temp_time=TA1R;
					  /*temp_time=TH1;
					  temp_time<<=8;
					  temp_time+=TL1;*/
					  if(temp_time>393)///修改此处；
					  start=1;
					  TA1R=0;KEY_value=0;//TH1=0;TL1=0;
				}
				else {I_count=0;TA1CTL&=~MC0;TA1R=0;KEY_value=0;}//TH1=0;TL1=0;如果没有第二次中断说明是干扰；
                          
		}
        }
}
///定时时间到。归零；
#pragma vector=TIMER1_A1_VECTOR
__interrupt void time1()
{
      switch(TA1IV)
      {
      case 2:break;
      case 4:break;
      case 14:I_count=0;TA1CTL&=~MC0;TA1R=0;start=0;break;///归零函数TR1=0;TH1=0;TL1=0;
      }

}
void key_decoding(uchar key_1)	///转换为16进制数；
{
	if((key_1/16)>9) buff[0]=key_1/16+0x37;///高八位检测；
	else buff[0]=key_1/16+0x30;
	if((key_1%16)>9) buff[1]=key_1%16+0x37;///低八位检测；
	else buff[1]=key_1%16+0x30;	
}

#endif 
