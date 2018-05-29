#include "stm32f10x.h" 
#include "stm32f10x_exti.h"
#include "delay.h"
#include "HCSR04.h"

u16 distance;
u8 distance_ok=0;
u8 flag_0=0;

void HCSR04_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBase_InitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_Period=50000;
	TIM_TimeBase_InitStructure.TIM_Prescaler=71;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBase_InitStructure);
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI9_5_IRQHandler(void)
{
	flag_0=~flag_0;
	if(flag_0)
	{
		distance_ok=0;
		TIM_Cmd(TIM2,ENABLE);
		TIM_SetCounter(TIM2,0);
	}
	else
	{
		TIM_Cmd(TIM2,DISABLE);
		distance=TIM_GetCounter(TIM2)*0.172;
		distance_ok=1;
	}
	EXTI_ClearFlag(EXTI_Line9);
}

void get_distance(void)
{
	//通道1第一次		
	  GPIO_SetBits(GPIOC,GPIO_Pin_8);
		delay_us(30);
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	  while(!distance_ok);
	  distance_ok=0;
}
		
