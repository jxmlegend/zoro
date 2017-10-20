#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "AD.h"

u16 AD_integ(void);
int main(void)
{		
  u16 TIMCounter,votage;
	delay_init();	    	 //��ʱ������ʼ��	 
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();
	AD_init();
	 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_SSD_BackLightSet(50);//��������
	LCD_Clear(BLACK);//��������ɫ
	LCD_ShowString(60,170,200,16,16,"TIM cnt is:000000");	
	LCD_ShowString(60,190,200,16,16,"AD voltage is: 0.000V");	
	TIM3_Int_Init(50000,71);//1Mhz�ļ���Ƶ�ʣ�������10000Ϊ10ms
   	while(1)
	{
		A_H;B_H;//һ��ʼ�õ��ݷŵ�11
		delay_ms(400);
		
		/*A_L;B_L;//��ģ�⿪��s0��ʼ���00
		delay_ms(6);//�ȴ�������
		
		//��ʼ������ִ򿪶�ʱ��
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
		A_L;B_H;//10
	
		//�жϵ�ѹ�Ƿ�ת  
		while(flag==0);
		
		//ֹͣ������ȡ����ֵ 
		TIMCounter = TIM_GetCounter(TIM3);
		TIM_Cmd(TIM3, DISABLE); //ʧ��TIMx		
		TIM_SetCounter(TIM3,0);*/
		
		//������ֵת���ɵ�ѹ
//		votage=5.0*TIMCounter/6;
		TIMCounter = AD_integ();
		if(TIMCounter>0&&TIMCounter<3575)
			votage=0.61*TIMCounter+325;
		else if(TIMCounter>3575&&TIMCounter<5230)
			votage=0.61*TIMCounter+305;
		else if(TIMCounter>5230)
			votage=0.61*TIMCounter+350;
		printf("��ʱ���ļ���ֵΪ��%d\r\n",TIMCounter);
		//LCD��ʾ����ֵ
		LCD_ShowxNum(148,170,TIMCounter,6,16,0x80);//��ʾ����ֵ
		//LCD��ʾ��ѹֵ
		LCD_ShowxNum(180,190,votage/1000,1,16,0x80);//��ʾ��ѹֵ
		LCD_ShowxNum(196,190,votage%1000,3,16,0x80);//��ʾ��ѹֵ
		LED0=!LED0;
		
	}	 
}	 
 
u16 AD_integ(void)
{
	u16 TIM_cnt;//��ʱ���ļ���ֵ
	if(sex==1)//����ѹ
	{
		A_L;B_L;//��ģ�⿪��s0��ʼ���//11��ʼ���
		delay_ms(10);//�ȴ�������
		//��ʼ������ִ򿪶�ʱ��
		TIM_Cmd(TIM3, ENABLE);//1Mhz�ļ���Ƶ�ʣ�������10000Ϊ10ms
		A_L;B_H;//10
		
		//�жϵ�ѹ�Ƿ�ת   //���ⲿ�ж�
		while(flag==0);
		
		//ֹͣ������ȡ����ֵ 
		TIM_Cmd(TIM3, DISABLE); //ʧ��TIMx
		
		TIM_cnt=TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3,0);
		LCD_ShowString(60,190,200,16,16,"AD voltage is:+0.000V");
		return TIM_cnt;
	}
	else//����ѹ
	{
		
		A_L;B_L;//��ģ�⿪��s0��ʼ���//11��ʼ���
		delay_ms(10);//�ȴ�������
		//��ʼ������ִ򿪶�ʱ��
		TIM_Cmd(TIM3, ENABLE);//1Mhz�ļ���Ƶ�ʣ�������10000Ϊ10ms
		A_H;B_L;//10
		
		//�жϵ�ѹ�Ƿ�ת
		while(flag==0);
		
		//ֹͣ������ȡ����ֵ  
		TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx
		
		TIM_cnt=TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3,0);
		LCD_ShowString(60,190,200,16,16,"AD voltage is:-0.000V");
		return TIM_cnt;
	}
}

