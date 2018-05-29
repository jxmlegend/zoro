#include "color_sensor.h"

/*******************************************
* 文件名： TCS3200颜色测试
* 型号：   GY-31
* 功能：   读取RGB值
* 单片机： STM32F103RET6
* 晶振：   11.0592m 
* 时间：   2016-7-6
* LCD1602显示R,G,B值
********************************************/

 //变量、常量定义
float rp=1.2,gp=1.6 ,bp=1.75; //定义比例因子，具体环境可以修改
u16 red_dat,green_dat,blue_dat,colorflag;
u32 red_buf,green_buf,blue_buf,red,green,blue;
u8 count=0;//颜色标志位（0:红 1:绿 2:蓝）

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能或者失能指定的TIM中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设						 
}


/*
   初始化颜色传感器  S2,S3 为输出接口，OUT接口配置成浮空输入
   定时器外部时钟触发
*/
void TIM4_Configuration(void)//只用一个外部秒冲端口
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
	
//第一步：设置为浮空模式	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	// PB0、PB1端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.6 
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.6
	
  GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);
//第二步：配置定时器
	TIM_DeInit(TIM4);//将定时器4的寄存器值设置默认值
	TIM_TimeBaseInitStructure.TIM_Period = 0xffff; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=0x00;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM4

	/* 配置TIMX外部时钟模式2
	输入参数2：TIM_TIxExternalCLK1Source_TI1  选定TI1作为触发输入源
	输入参数3：TIM_ICPolarity_Rising  上升沿有效
	输入参数4：0  输入比较滤波器 0x0~0xf
	*/
	TIM_TIxExternalClockConfig(TIM4,TIM_TIxExternalCLK1Source_TI1,TIM_ICPolarity_Rising,0);
	
	TIM_SetCounter(TIM4,0);//设置TIM4计数器寄存器值为0
	TIM_Cmd(TIM4,ENABLE);//使能TIM4外设
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
  TIM_Cmd(TIM3,DISABLE);  //失能TIM3外设  关闭定时
	TIM_Cmd(TIM4,DISABLE);  //失能TIM4外设  关闭计数
	 
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{	
     if(count==0)
		{
			 count++;
			 S2=1;S3=1;	  //选择滤波器为绿色     
			 red_dat=(u16)TIM_GetCounter(TIM4)*rp;//上一次 开红色滤波器的值
			red_buf+=red_dat;
		}
		else if(count==1)
		{
			 count++;
			 S2=0;S3=1;//选择滤波器为蓝色
			 green_dat=(u16)TIM_GetCounter(TIM4)*gp;//上一次 开绿色滤波器的值
			green_buf+=green_dat;
		}else if(count==2)
		{
			 count=0;
			GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);
			 S2=0;S3=0;   //选择滤波器为红色
			 blue_dat=(u16)TIM_GetCounter(TIM4)*bp;//上一次 开蓝色滤波器的值
			blue_buf+=blue_dat;
			colorflag+=1;
		}	 
	}	 
		 TIM_SetCounter(TIM4,0);//计数器值清零
		 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
		 TIM_Cmd(TIM3,ENABLE); 
		 TIM_Cmd(TIM4,ENABLE); 
	if(colorflag==9)
	{
			red=red_buf;
			green=green_buf;
			blue=blue_buf;
			red_buf=0;
			green_buf=0;
			blue_buf=0;
			colorflag=0;
	}
}




