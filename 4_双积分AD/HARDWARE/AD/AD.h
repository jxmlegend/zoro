#ifndef __AD_H
#define __AD_H	 
#include "sys.h"
#include "stm32f10x.h"
#define sex GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)//读取极性判断//高->高
#define flag GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)//读取T2开始计数标志//高->高
#define A_L         GPIOA->BSRR =  GPIO_Pin_6 //高
#define A_H         GPIOA->BRR =  GPIO_Pin_6//低

//#define A_L         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
//#define A_H         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);

#define B_L         GPIOA->BSRR =  GPIO_Pin_5 
#define B_H         GPIOA->BRR =  GPIO_Pin_5

//#define B_L         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET);
//#define B_H         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);

void AD_init(void);

#endif

