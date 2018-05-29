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
	 Infrared_IO_init();//初始化红外发射端口
	 delay_init();	    	 //延时函数初始化	  
	  LED_Init();		  	//初始化与LED连接的硬件接口
	  MotorInit(7199,0); //=====初始化PWM 10KHZ，用于驱动电机
	 	XUNJI_GPIO_init();	
  	SetMotorPWM(5000,5000);
  	delay_ms(2000);
	 	uart_init(9600);
	  LED0=0;		 
	 	NRF24L01_Init();
		NRF24L01_FindMyself();	 
		NRF24L01_TX_Mode();
	  Infrared_IO_init();//初始化红外发射端口
		KEY_Init();
		Init_BH1750();
	 	NVIC_Configuration();
		TIM4_Configuration();//配置PB6为计数输入口
		S2=0;S3=0;//开红色	
		TIM3_Int_Init(100,7199);//10Khz的计数频率，计数到100为10ms  
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
 

