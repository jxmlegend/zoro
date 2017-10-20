#include "exti.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "lcd.h"
#include "timer.h"
#include "usart.h"

u8 direction_flag1,direction_flag2,start1,start2;//�����жϱ�־
u8 swing1,swing2;
u8 ball_cnt1,ball_cnt2;//�������
u16 TIM_cnt1,TIM_cnt2,TIM_cnt3;
u16 v1,v2;
u16 angle,a,start_1=0,start_2=0;
u16 TIM_temp,Real_time;
extern u8 speed_mode;
extern u8 flag1,flag2;

//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/*ʱ�ӳ�ʼ��*/
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//ʹ��PORTAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
	/*GPIO��ʼ��*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIOA2,3,4,5
	
	/*�ж���������*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource1);//GPIOA.1 �ж����Լ��жϳ�ʼ������   �½��ش���
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//A.1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	//GPIOG.2	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
//	//GPIOG.3	  �ж����Լ��жϳ�ʼ������ �����ش��� 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource3);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line3;	//A.1
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
////	//GPIOG.4	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource4);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//G.4
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	/*NVIC����*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�1 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
////	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;					//�����ȼ�1 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
}
//�ⲿ�ж�1�������
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);//���EXTI��·����λ
		start2=1;
		flag1=1;
		if(speed_mode==1)
		{
			if(start1==0)
			{
				TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
				TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
				start1=1;
			}
			else 
			{
				TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
				TIM_Cmd(TIM3, DISABLE);  //ʹ��TIM3
				TIM_cnt2=TIM_GetCounter(TIM3);
				TIM_SetCounter(TIM3,0);
				LCD_ShowxNum(30,230,TIM_cnt2,6,16,0x80);//��er������
				start1=0;
			}
			
		}
		if(speed_mode==2)
		{
			if((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)==0))
			{
				while((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)==0));
				swing1++;
				LCD_ShowxNum(30,230,swing1,2,16,0x80);
			}
		}
	}
}
//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);//���EXTI��·����λ
		flag1=0;
		start1=1;
		if(speed_mode==1)
		{
			if(start2==0)
			{
				TIM_Cmd(TIM2, DISABLE);  //ʹ��TIM2
				TIM_cnt1=TIM_GetCounter(TIM2);
				TIM_SetCounter(TIM2,0);
				LCD_ShowxNum(30,210,TIM_cnt1,6,16,0x80);//��һ������
				start2=1;
			}
			else
			{
				TIM_Cmd(TIM5, DISABLE);  //ʹ��TIM3
				TIM_cnt3=TIM_GetCounter(TIM5);
				TIM_SetCounter(TIM5,0);
				LCD_ShowxNum(30,250,TIM_cnt3,6,16,0x80);//��3������
				start2=0;
			}
			//����
//			if(start2==1)
//			{
//				direction_flag2=0;
//				start2=0;
//			}
//			else
//				direction_flag2=1;					
		}
		if(speed_mode==2)
		{
			if((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)==0))
			{
				while((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)==0));
				swing2++;
				LCD_ShowxNum(100,230,swing2,2,16,0x80);
			}
		}
	} 
}
//void EXTI3_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line3)==SET)
//	{
//		TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
//		TIM_Cmd(TIM3, DISABLE);  //ʹ��TIM3
//		EXTI_ClearITPendingBit(EXTI_Line3);//���EXTI��·����λ
//		TIM_cnt2=TIM_GetCounter(TIM3);
//		TIM_SetCounter(TIM3,0);
//		LCD_ShowxNum(30,230,TIM_cnt2,6,16,0x80);//��er������
//	}
//}

//void EXTI4_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line4)==SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line4);//���EXTI��·����λ
//		TIM_Cmd(TIM3, DISABLE);  //ʹ��TIM3
//		TIM_cnt3=TIM_GetCounter(TIM5);
//		TIM_SetCounter(TIM5,0);
//		LCD_ShowxNum(30,250,TIM_cnt3,6,16,0x80);//��3������
//	}
//}
 
