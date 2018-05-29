#include "color_sensor.h"

/*******************************************
* �ļ����� TCS3200��ɫ����
* �ͺţ�   GY-31
* ���ܣ�   ��ȡRGBֵ
* ��Ƭ���� STM32F103RET6
* ����   11.0592m 
* ʱ�䣺   2016-7-6
* LCD1602��ʾR,G,Bֵ
********************************************/

 //��������������
float rp=1.2,gp=1.6 ,bp=1.75; //����������ӣ����廷�������޸�
u16 red_dat,green_dat,blue_dat,colorflag;
u32 red_buf,green_buf,blue_buf,red,green,blue;
u8 count=0;//��ɫ��־λ��0:�� 1:�� 2:����

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//ʹ�ܻ���ʧ��ָ����TIM�ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����						 
}


/*
   ��ʼ����ɫ������  S2,S3 Ϊ����ӿڣ�OUT�ӿ����óɸ�������
   ��ʱ���ⲿʱ�Ӵ���
*/
void TIM4_Configuration(void)//ֻ��һ���ⲿ���˿�
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��
	
//��һ��������Ϊ����ģʽ	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	// PB0��PB1�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.6 
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 // �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.6
	
  GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);
//�ڶ��������ö�ʱ��
	TIM_DeInit(TIM4);//����ʱ��4�ļĴ���ֵ����Ĭ��ֵ
	TIM_TimeBaseInitStructure.TIM_Period = 0xffff; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=0x00;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4

	/* ����TIMX�ⲿʱ��ģʽ2
	�������2��TIM_TIxExternalCLK1Source_TI1  ѡ��TI1��Ϊ��������Դ
	�������3��TIM_ICPolarity_Rising  ��������Ч
	�������4��0  ����Ƚ��˲��� 0x0~0xf
	*/
	TIM_TIxExternalClockConfig(TIM4,TIM_TIxExternalCLK1Source_TI1,TIM_ICPolarity_Rising,0);
	
	TIM_SetCounter(TIM4,0);//����TIM4�������Ĵ���ֵΪ0
	TIM_Cmd(TIM4,ENABLE);//ʹ��TIM4����
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
  TIM_Cmd(TIM3,DISABLE);  //ʧ��TIM3����  �رն�ʱ
	TIM_Cmd(TIM4,DISABLE);  //ʧ��TIM4����  �رռ���
	 
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{	
     if(count==0)
		{
			 count++;
			 S2=1;S3=1;	  //ѡ���˲���Ϊ��ɫ     
			 red_dat=(u16)TIM_GetCounter(TIM4)*rp;//��һ�� ����ɫ�˲�����ֵ
			red_buf+=red_dat;
		}
		else if(count==1)
		{
			 count++;
			 S2=0;S3=1;//ѡ���˲���Ϊ��ɫ
			 green_dat=(u16)TIM_GetCounter(TIM4)*gp;//��һ�� ����ɫ�˲�����ֵ
			green_buf+=green_dat;
		}else if(count==2)
		{
			 count=0;
			GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);
			 S2=0;S3=0;   //ѡ���˲���Ϊ��ɫ
			 blue_dat=(u16)TIM_GetCounter(TIM4)*bp;//��һ�� ����ɫ�˲�����ֵ
			blue_buf+=blue_dat;
			colorflag+=1;
		}	 
	}	 
		 TIM_SetCounter(TIM4,0);//������ֵ����
		 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
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




