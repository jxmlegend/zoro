#include "exti.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "lcd.h"
#include "timer.h"
#include "usart.h"

u8 direction_flag1,direction_flag2,start1,start2;//方向判断标志
u8 swing1,swing2;
u8 ball_cnt1,ball_cnt2;//钢珠计数
u16 TIM_cnt1,TIM_cnt2,TIM_cnt3;
u16 v1,v2;
u16 angle,a,start_1=0,start_2=0;
u16 TIM_temp,Real_time;
extern u8 speed_mode;
extern u8 flag1,flag2;

//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/*时钟初始化*/
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//使能PORTA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIOA2,3,4,5
	
	/*中断向量配置*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource1);//GPIOA.1 中断线以及中断初始化配置   下降沿触发
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//A.1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	//GPIOG.2	  中断线以及中断初始化配置 下降沿触发 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//	//GPIOG.3	  中断线以及中断初始化配置 上升沿触发 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource3);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line3;	//A.1
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
////	//GPIOG.4	  中断线以及中断初始化配置 下降沿触发 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource4);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//G.4
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	/*NVIC配置*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY1所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能按键KEY2所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//使能按键KEY1所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级1 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
////	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键KEY1所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;					//子优先级1 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
}
//外部中断1服务程序
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);//清除EXTI线路挂起位
		start2=1;
		flag1=1;
		if(speed_mode==1)
		{
			if(start1==0)
			{
				TIM_Cmd(TIM2, ENABLE);  //使能TIM2
				TIM_Cmd(TIM3, ENABLE);  //使能TIM3
				start1=1;
			}
			else 
			{
				TIM_Cmd(TIM5, ENABLE);  //使能TIM5
				TIM_Cmd(TIM3, DISABLE);  //使能TIM3
				TIM_cnt2=TIM_GetCounter(TIM3);
				TIM_SetCounter(TIM3,0);
				LCD_ShowxNum(30,230,TIM_cnt2,6,16,0x80);//第er个脉宽
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
//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);//清除EXTI线路挂起位
		flag1=0;
		start1=1;
		if(speed_mode==1)
		{
			if(start2==0)
			{
				TIM_Cmd(TIM2, DISABLE);  //使能TIM2
				TIM_cnt1=TIM_GetCounter(TIM2);
				TIM_SetCounter(TIM2,0);
				LCD_ShowxNum(30,210,TIM_cnt1,6,16,0x80);//第一个脉宽
				start2=1;
			}
			else
			{
				TIM_Cmd(TIM5, DISABLE);  //使能TIM3
				TIM_cnt3=TIM_GetCounter(TIM5);
				TIM_SetCounter(TIM5,0);
				LCD_ShowxNum(30,250,TIM_cnt3,6,16,0x80);//第3个脉宽
				start2=0;
			}
			//方向
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
//		TIM_Cmd(TIM5, ENABLE);  //使能TIM5
//		TIM_Cmd(TIM3, DISABLE);  //使能TIM3
//		EXTI_ClearITPendingBit(EXTI_Line3);//清除EXTI线路挂起位
//		TIM_cnt2=TIM_GetCounter(TIM3);
//		TIM_SetCounter(TIM3,0);
//		LCD_ShowxNum(30,230,TIM_cnt2,6,16,0x80);//第er个脉宽
//	}
//}

//void EXTI4_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line4)==SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line4);//清除EXTI线路挂起位
//		TIM_Cmd(TIM3, DISABLE);  //使能TIM3
//		TIM_cnt3=TIM_GetCounter(TIM5);
//		TIM_SetCounter(TIM5,0);
//		LCD_ShowxNum(30,250,TIM_cnt3,6,16,0x80);//第3个脉宽
//	}
//}
 
