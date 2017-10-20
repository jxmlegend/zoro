/*-------------------------------------------------------------------------------
�ļ����ƣ�main.c
�ļ�������ͨ������1����ӡADC1��ѹֵ��PC1��
Ӳ��ƽ̨����ĪM3S������
��д����shifang
�̼���  ��V3.5
������̳��www.doflye.net
��    ע��ͨ�����޸Ŀ�����ֲ�����������壬����������Դ�����硣
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "delay.h"
#include "pwm.h"
#include "usart.h"
#include "adc.h"

// ADC1ת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ			 
float ADC_ConvertedValueLocal;     
float sw;
int main(void)
{
	u32 flag                                                                                                          ;
  SysTick_Init();//��ʱ��ʼ��
  USART1_Int(9600);
	ADC1_Init();
	pwm_init();
	
  printf("\n\rADC Test: ˮλ��ѹ��\r");
  while (1)
  {				
		
	//ʹ��printf����ѭ�����͹̶���Ϣ
		Delay_ms(20);
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // ��ȡת����ADֵ
		flag=ADC_ConvertedValueLocal*1308+200;//1308
		sw=(-1.5)*ADC_ConvertedValueLocal+2.1;//-1.5
		printf("\r\n �ɼ���ֵ16���� = 0x%04X \r\n", ADC_ConvertedValue); 
		printf("\r\n ת�����ʵ�ʵ�ѹֵ = %f V \r\n",ADC_ConvertedValueLocal); 
		printf("\r\n ˮλ = %f mL \r\n",sw); 
		TIM_SetCompare2(TIM3,flag);//����TIMx����Ƚ�2�Ĵ���ֵ
		Delay_ms(500);


  }
}


