#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "pwm.h"

 int main(void)
 {	 
  u16 adcx;
	u32 flag;
	float water;
	float temp;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
 	Adc_Init();		  		//ADC��ʼ��
	pwm_init();

	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"ADC TEST");	
	LCD_ShowString(60,110,200,16,16,"2017/7/4");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,200,16,16,"ADC_CH0_VOL:0.000V");	
	LCD_ShowString(60,170,200,16,16,"water is:0000ml");	
	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ
		temp=(float)adcx*(3.3/4096);
		printf("AD vla:%f\r\n",temp);
		flag=temp*1308+200;
		TIM_SetCompare2(TIM3,flag);//����TIMx����Ƚ�2�Ĵ���ֵ
		adcx=temp;
		water=(-1615)*temp+2100;
		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ
		LCD_ShowxNum(132,170,water,4,16,0x80);//��ʾˮλֵ
		
		//����С������
		temp-=adcx;
		temp*=1000;
		LCD_ShowxNum(172,150,temp,3,16,0X80);
		LED0=!LED0;
		delay_ms(250);	
	}
 }

