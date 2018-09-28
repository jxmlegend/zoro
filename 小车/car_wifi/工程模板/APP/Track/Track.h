#ifndef _Track_H
#define _Track_H
#include "stm32f10x.h"
#define left1_H         GPIOG->BSRR =  GPIO_Pin_10 //高
#define left1_L         GPIOG->BRR =  GPIO_Pin_10//低

#define left2_H         GPIOG->BSRR =  GPIO_Pin_11 //高
#define left2_L         GPIOG->BRR =  GPIO_Pin_11//低

#define right1_H         GPIOG->BSRR =  GPIO_Pin_12 //高
#define right1_L         GPIOG->BRR =  GPIO_Pin_12//低

#define right2_H         GPIOG->BSRR =  GPIO_Pin_13 //高
#define right2_L         GPIOG->BRR =  GPIO_Pin_13//低

//#define en1_H         GPIOG->BSRR =  GPIO_Pin_14 //高
//#define en1_L         GPIOG->BRR =  GPIO_Pin_14//低

//#define en2_H         GPIOG->BSRR =  GPIO_Pin_15 //高
//#define en2_L         GPIOG->BRR =  GPIO_Pin_15//低


void straight(void); //直行函数
void turn_left(void); //左转调整函数
void turn_right(void); //右转调整函数
void turn_back(void); //倒车函数
void turn_stop(void); //停车函数
void infrared(void); //循迹函数
	
void Track_init(void);



#endif
