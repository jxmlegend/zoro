#include "led.h"
#include "delay.h"
#include "sys.h"
#include "motor_pwm.h"
#include "xunji.h"
#include "usart.h"
#include "ZigBee.h"
#include "24l01.h"
#include "RC.h"
#include "Infrared_emsiion.h"
#include "key.h"
#include "BH1750.h"
#include "i2c.h"
#include "color_sensor.h"
#include "HCSR04.h"


extern u32 red,green,blue;
extern u16 distance;
u16 stddis;

 int main(void)
 {	
	 Infrared_IO_init();//��ʼ�����ⷢ��˿�
	 delay_init();	    	 //��ʱ������ʼ��	  
	  LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	  MotorInit(7199,0); //=====��ʼ��PWM 10KHZ�������������
	 	XUNJI_GPIO_init();	
  	SetMotorPWM(5000,5000);
  	delay_ms(2000);
	 	uart_init(9600);
	  LED0=0;		 
	 	NRF24L01_Init();
		NRF24L01_FindMyself();	 
		NRF24L01_TX_Mode();
	  Infrared_IO_init();//��ʼ�����ⷢ��˿�
		KEY_Init();
		Init_BH1750();
	 	NVIC_Configuration();
		TIM4_Configuration();//����PB6Ϊ���������
		S2=0;S3=0;//����ɫ	
		TIM3_Int_Init(100,7199);//10Khz�ļ���Ƶ�ʣ�������100Ϊ10ms  
		HCSR04_init();
		stddis=100;
		while(1)
		{
			while(Alert_flag>12)
				MOTOR_GO_STOP;
			get_distance();	
			if(distance<stddis)
			{
					MOTOR_GO_STOP;
			}
			else
			{
						FollowLine();
					car_task(Alert_flag);
			}	
			
		}

}
 

