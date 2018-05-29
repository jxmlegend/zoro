#include "motor_pwm.h"
#include "led.h"
 


//初始化电机控制所需IO
void Motor_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3 ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3);
}

//TIM8的通道1、2设置成PWM输出端口
static void TIM8_Mode_Config(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Time base configuration */		 
	TIM_TimeBaseStructure.TIM_Period = arr;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = psc;	    //设置预分频：不预分频，即为36MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平


	TIM_OC1Init(TIM8, &TIM_OCInitStructure);	//根据TIM_OCInitStruct中指定的参数初始化外设TIMx 
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OC2Init(TIM8, &TIM_OCInitStructure); 	

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);//TIM8在CCR1上的预装载寄存器使能
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);//TIM8在CCR2上的预装载寄存器使能

	
	TIM_ARRPreloadConfig(TIM8, ENABLE);	  // 使能TIM8重载寄存器ARR
	TIM8->BDTR=0X8000;
	TIM_Cmd(TIM8, ENABLE);
}


void MotorInit(u16 arr,u16 psc)
{
	Motor_GPIO_Config();
	TIM8_Mode_Config(arr,psc);	
}
void SetMotorPWM(u16 PWM1,u16 PWM2)
{
 	TIM_SetCompare1(TIM8,PWM1);	 
	TIM_SetCompare2(TIM8,PWM2);
}
