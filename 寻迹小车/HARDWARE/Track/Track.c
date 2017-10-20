#include "Track.h"
#include "pwm.h"
#include "delay.h"

void Track_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=	GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5
															| GPIO_Pin_6| GPIO_Pin_7;//传感器输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13 ;//正反转
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14 |GPIO_Pin_15;//使能
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
//	GPIO_Init(GPIOG,&GPIO_InitStructure);
}

void straight() //直行函数//01前  10前
{
//	pro_right=20;
//	pro_left=20;
//	left1=0;
//	left2=1;
//	right1=0;
//	right2=1;
	
	left1_L;
	left2_H;
	right1_H;
	right2_L;
	TIM_SetCompare1(TIM3,5000);//设置TIMx捕获比较1寄存器值
	TIM_SetCompare2(TIM3,5000);//设置TIMx捕获比较2寄存器值
}
void turn_left() //左转调整函数
{
//	pro_right=30;
//	pro_left=30;
//	left1=1;
//	left2=0;
//	right1=0;
//	right2=1;
	
	left1_H;
	left2_L;
	right1_H;
	right2_L;
	TIM_SetCompare1(TIM3,5000);//设置TIMx捕获比较1寄存器值
	TIM_SetCompare2(TIM3,5000);//设置TIMx捕获比较2寄存器值
}
void turn_right() //右转调整函数
{
//	pro_right=30;
//	pro_left=30;
//	left1=0;
//	left2=1;
//	right1=1;
//	right2=0;
	
	left1_L;
	left2_H;
	right1_L;
	right2_H;
	TIM_SetCompare1(TIM3,5000);//设置TIMx捕获比较1寄存器值
	TIM_SetCompare2(TIM3,5000);//设置TIMx捕获比较2寄存器值
}
void turn_back() //倒车函数
{
//	left1=1;
//	left2=0;
//	right1=1;
//	right2=0;
//	pro_right=20;
//	pro_left=20;
	
	left1_H;
	left2_L;
	right1_L;
	right2_H;
	TIM_SetCompare1(TIM3,5000);//设置TIMx捕获比较1寄存器值
	TIM_SetCompare2(TIM3,5000);//设置TIMx捕获比较2寄存器值
}
void turn_stop() //停车函数
{
//	left1=0;
//	left2=0;
//	right1=0;
//	right2=0;
//	pro_right=0;
//	pro_left=0;	 

	left1_L;
	left2_L;
	right1_L;
	right2_L;
	TIM_SetCompare1(TIM3,0);//设置TIMx捕获比较1寄存器值
	TIM_SetCompare2(TIM3,0);//设置TIMx捕获比较2寄存器值
}

void infrared() //循迹函数
{
	u8 stop;//停车标志
	u8 temp;//临时标志
	u8 flag;
	u16 IN = GPIO_ReadInputData(GPIOG);
	IN = IN%256;
	if(IN==0xff) stop=1;
	else stop=0;

	while(!stop) //没有循迹停止，一直循迹下去
	{	
		IN = GPIO_ReadInputData(GPIOG);
		IN = IN%256;
		switch(IN)
		{
			//居中状态
			case 0x10: stop=0;flag=0;break;	  //00010000
			case 0x08: stop=0;flag=0;break;	  //00001000
			case 0x18: stop=0;flag=0;break;   //00011000
			//偏右状态
			case 0x20: stop=0;flag=1;break;	  //00100000
			case 0x40: stop=0;flag=1;break;	  //01000000
			case 0x80: stop=0;flag=1;break;	  //10000000
			case 0x30: stop=0;flag=1;break;	  //00110000
			case 0x60: stop=0;flag=1;break;	  //01100000
			case 0xc0: stop=0;flag=1;break;	  //11000000
			
			//偏左状态
			case 0x04: stop=0;flag=2;break;	  //00000100
			case 0x02: stop=0;flag=2;break;	  //00000010
			case 0x01: stop=0;flag=2;break;	  //00000001
			case 0x03: stop=0;flag=2;break;	  //00001100
			case 0x06: stop=0;flag=2;break;	  //00000110
			case 0x0c: stop=0;flag=2;break;	  //00000011
			//循到路口黑线 left_offset=1; right_offset=1;mid_offset=1;
			case 0xFF: stop=1;flag=4; break;
			case 0x00: stop=0;flag=3;break;//出界
			default: break;
		}
		if(stop)
		{
			turn_stop(); delay_ms(100);
		}
		else
		{	 
			switch (flag)
			{
				case 0: straight();	  	break;
				case 1: turn_left();  	break;
				case 2: turn_right(); 		break;
				case 3: turn_back();  	break;
				case 4: turn_stop();  	break;	 	 			
				default:   		 		break;
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
