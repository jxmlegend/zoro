#ifndef _XUNJI_H_
#define _XUNJI_H_

#include "sys.h"


#define   XUNJI0    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)  //��ȡѭ��0
#define   XUNJI1    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)  //��ȡѭ��1
#define   XUNJI2    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) //��ȡѭ��2
#define   XUNJI3    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) //��ȡѭ��3
#define   XUNJI4    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) //��ȡѭ��4
#define   XUNJI5    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//��ȡѭ��5
#define   XUNJI6    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//��ȡѭ��6
#define   XUNJI7    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//��ȡѭ��7

#define   XUNJI    (XUNJI0+XUNJI1<<1+XUNJI2<<2+XUNJI3<<3+XUNJI4<<4+XUNJI5<<5+XUNJI6<<6+XUNJI1<<7)

//ѭ��IO�ڳ�ʼ��
void XUNJI_GPIO_init(void);
//ѭ�� ��ɫ����
void  FollowLine(void);

extern volatile u8 xj,Alert_flag;
void car_task(u8 crossroads_count);
void ADC_capture_task(uchar Standard_Leve);

#endif


