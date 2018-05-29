#include "NRF24L01.H"
#include "commom.h"
#include "Infrared_emission.h"
#include "motor_xunji.h"
#include "motor.h"
#include "STC12C5A60S2_PWM.h"
#include "delay.h"
#include "color_sensor.h"
#include "RC.h"
#include "uart.h"
#include "HCSR04.h"

u16 stddis;

void main(void)
{  
	P4SW|=0X10;        //将 P4.4口设置成IO口,准双向口
	 P4M1&=~0X10;
	 P4M0&=~0X10;
	 BEEP=1;
	
	 Motor_Init(); 
	 PWM_ini();

	 PWM_Set(70,70);   //PWM使用FOSC作为时钟源所以PWM设置可以只执行一次
	 Delay_1ms(50);
   s2=0;           //初始设定S2引脚
	 s3=0;           //初始设定S3引脚
	 t0_init();
	 UartInit();
		init_NRF24L01();        
   NRF24L01_FindMyself();         	//2401自检 
	 NRF24L01_TX_Mode(0x00,0x04);             //发送模式
	 NRF24L01_transmit(0x00);
	 NRF24L01_transmit(0x00);
		P4SW = 0x70;
		stddis=1;
		
	while(1)
  {  			 		      
		  while(Alert_flag>11)
				MOTOR_GO_STOP;
				if(distance()>stddis)
				{
					FollowLine();	
					car_task(Alert_flag);
					if(Alert_flag==6)//只在第6个路口和 第7个路口之间  颜色检测
					{	
					 while((red_dat>green_dat))
						{ 
							 PWM_Set(100,100); //小车减速
							 P17=0;
							 delay_ms(250);
							 P17=1;
							 delay_ms(250);
							 MOTOR_GO_STOP;
							 delay_ms(250);
							 while(red_dat>green_dat);						
						 }
							PWM_Set(70,70);
					}
				}
				else
				{MOTOR_GO_STOP;}
   }
}

