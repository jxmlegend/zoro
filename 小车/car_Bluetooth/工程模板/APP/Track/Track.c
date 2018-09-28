#include "Track.h"
#include "pwm.h"
#include "systick.h"
/*------------------------------------------
 函数功能:初始化寻迹车的控制引脚
 函数参数:无
 函数说明:无			
------------------------------------------*/
void Track_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=	GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4;//传感器输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13 ;//正反转
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOG,&GPIO_InitStructure);
}
/*------------------------------------------
 函数功能:直行函数
 函数参数:
 函数说明: 				
------------------------------------------*/
void straight(void)//01前  10前
{	
	left1_H;
	left2_L;
	right1_H;
	right2_L;
//	TIM_SetCompare1(TIM3,5000);//设置车轮转速
//	TIM_SetCompare2(TIM3,5000);//设置车轮转速
//	TIM_SetCompare3(TIM3,1500);
}
/*------------------------------------------
 函数功能:左转调整函数
 函数参数:
 函数说明: 				
------------------------------------------*/
void turn_left(void)
{
	left1_H;
	left2_L;
	right1_H;
	right2_L;
//	TIM_SetCompare1(TIM3,5000);//设置车轮转速
//	TIM_SetCompare2(TIM3,5000);//设置车轮转速
//	TIM_SetCompare3(TIM3,1100);
}
/*------------------------------------------
 函数功能:右转调整函数
 函数参数:
 函数说明: 				
------------------------------------------*/
void turn_right(void)
{
	left1_H;
	left2_L;
	right1_H;
	right2_L;
//	TIM_SetCompare1(TIM3,5000);//设置车轮转速
//	TIM_SetCompare2(TIM3,5000);//设置车轮转速
//	TIM_SetCompare3(TIM3,1900);
}
/*------------------------------------------
 函数功能:倒退调整函数
 函数参数:
 函数说明: 				
------------------------------------------*/
void turn_back(void)
{
	left1_L;
	left2_H;
	right1_L;
	right2_H;
//	TIM_SetCompare1(TIM3,5000);//设置车轮转速
//	TIM_SetCompare2(TIM3,5000);//设置车轮转速
//	TIM_SetCompare3(TIM3,1500);
}
/*------------------------------------------
 函数功能:停车函数
 函数参数:
 函数说明: 				
------------------------------------------*/
void turn_stop(void) 
{
	left1_L;
	left2_L;
	right1_L;
	right2_L;
//	TIM_SetCompare1(TIM3,0);//设置车轮转速
//	TIM_SetCompare2(TIM3,0);//设置车轮转速
//	TIM_SetCompare3(TIM3,1500);
}
/*------------------------------------------
 函数功能:小车循迹函数
 函数说明:小车有5个红外探头				
------------------------------------------*/
void infrared() //循迹函数
{
	u8 stop;//停车标志
	u8 flag;
	u8 IN = GPIO_ReadInputData(GPIOE)&0x1f;
//	IN = IN%32;
	if(IN==0x00) stop=1;
	else stop=0;

	while(!stop) //没有循迹停止，一直循迹下去
	{	
		IN = GPIO_ReadInputData(GPIOE)&0x1f;
//		IN = IN%32;
		switch(IN)
		{
			//居中状态												//黑线			白线
			case 0x04: stop=0;flag=0;break;	  //00010000  00100
			case 0x0c: stop=0;flag=0;break;	  //00001000	01100
			case 0x0e: stop=0;flag=0;break;   //00011000	01110
			case 0x1f: stop=0;flag=0;break;   //00011000	11111
			//微偏左状态
			case 0x0f: stop=0;flag=1;break;	  //00100000	01111
			case 0x07: stop=0;flag=1;break;	  //01000000	00111
			//偏左状态
			case 0x03: stop=0;flag=2;break;	  //00100000	00011
			case 0x01: stop=0;flag=2;break;	  //01000000	00001
			//微偏右状态
			case 0x1e: stop=0;flag=3;break;	  //00000110  11110
			case 0x1c: stop=0;flag=3;break;	  //00000011	11100
			//偏右状态
			case 0x18: stop=0;flag=4;break;	  //00000110  11000
			case 0x10: stop=0;flag=4;break;	  //00000011	10000
			//出界
			case 0x00: stop=1;flag=5;break;
			default:   stop=0;flag=0;break;
		}
		if(stop)
		{
			turn_back(); 
			TIM_SetCompare3(TIM3,1500);
			delay_ms(100);
		}
		else
		{	 
			switch (flag)
			{
				case 0: straight();TIM_SetCompare3(TIM3,1500);delay_ms(100);		break;//直行
				
				case 1: turn_right();TIM_SetCompare3(TIM3,1800);delay_ms(100); 	break;//微偏左
				case 2: turn_right();TIM_SetCompare3(TIM3,2000);delay_ms(100);	break;//偏左
				
				case 3: turn_left();TIM_SetCompare3(TIM3,1200);delay_ms(100);  	break;//微偏右
				case 4: turn_left();TIM_SetCompare3(TIM3,1000);delay_ms(100);  	break;//偏右	
				
				case 5: turn_back();TIM_SetCompare3(TIM3,1500);delay_ms(100);  	break;//出界
				default:   		 					break;
			}
		} 
	}
}

//void infrared()
//{
//	u16 temp_b,temp;
//		temp_b=GPIO_ReadInputData(GPIOG);
//		temp_b=temp_b%256;
//	if(temp_b==0xff)
//	{
//		temp=3;turn_stop();delay_ms(100);
//	}
//	else
//		temp=1;
//	while(temp!=3)
//	{
//		temp_b=GPIO_ReadInputData(GPIOG);
//		temp_b=temp_b%256;
//		switch(temp_b)
//		{
//			case 0x08:temp=1;break;	//00001000
//			case 0x18:temp=1;break;	//00011000
//			case 0x10:temp=1;break;	//00010000
//			//居中
//			case 0x80:temp=0;break;	//10000000
//			case 0x40:temp=0;break;	//01000000
//			case 0x20:temp=0;break;	//00100000
//			//偏左
//			case 0x01:temp=2;break;	//00000001
//			case 0x02:temp=2;break;	//00000010
//			case 0x04:temp=2;break;	//00000100
//			//偏右
//			case 0xff:temp=3;turn_stop();delay_ms(100);break; //11111111
//			case 0x7f:temp=3;turn_stop();delay_ms(100);break; //01111111
//			case 0xfe:temp=3;turn_stop();delay_ms(100);break; //11111110
//			case 0x3f:temp=3;turn_stop();delay_ms(100);break; //00111111
//			case 0xfC:temp=3;turn_stop();delay_ms(100);break; //11111100
//			case 0x7e:temp=3;turn_stop();delay_ms(100);break; //01111110
//			case 0x7c:temp=3;turn_stop();delay_ms(100);break; //01111100
//			case 0x3e:temp=3;turn_stop();delay_ms(100);break; //00111110
//			case 0x5e:temp=3;turn_stop();delay_ms(100);break; //01011110
//			case 0x7a:temp=3;turn_stop();delay_ms(100);break; //01111010
//			case 0x79:temp=3;turn_stop();delay_ms(100);break; //01111001
//			case 0x9e:temp=3;turn_stop();delay_ms(100);break; //10011110
//			//停车
//			case 0x00:temp=4;break;	//00000000
//			//过界
//			default:break;
//		}
//		switch(temp)
//		{
//			case 0:turn_left();break;
//			case 1:straight();break;
//			case 2:turn_right();break;
//			case 3:turn_stop();break;
//			case 4:turn_back();break;
//			default:break;
//		}
//	}	
//}
