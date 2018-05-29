#ifndef _COLOR_SENSOR_H_
#define _COLOR_SENSOR_H_

#include "sys.h"

#define S2 PBout(0)	// PB0
#define S3 PBout(3)	// PB3


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

void TIM3_Int_Init(u16 arr,u16 psc);
/*
   初始化颜色传感器  S2,S3 为输出接口，OUT接口配置成浮空输入
   定时器外部时钟触发
*/
void TIM4_Configuration(void);//只用一个外部秒冲端口

extern u16 red_dat,green_dat,blue_dat;
#endif

