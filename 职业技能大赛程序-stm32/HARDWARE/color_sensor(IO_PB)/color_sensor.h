#ifndef _COLOR_SENSOR_H_
#define _COLOR_SENSOR_H_

#include "sys.h"

#define S2 PBout(0)	// PB0
#define S3 PBout(3)	// PB3


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

void TIM3_Int_Init(u16 arr,u16 psc);
/*
   ��ʼ����ɫ������  S2,S3 Ϊ����ӿڣ�OUT�ӿ����óɸ�������
   ��ʱ���ⲿʱ�Ӵ���
*/
void TIM4_Configuration(void);//ֻ��һ���ⲿ���˿�

extern u16 red_dat,green_dat,blue_dat;
#endif

