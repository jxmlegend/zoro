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
	P4SW|=0X10;        //�� P4.4�����ó�IO��,׼˫���
	 P4M1&=~0X10;
	 P4M0&=~0X10;
	 BEEP=1;
	
	 Motor_Init(); 
	 PWM_ini();

	 PWM_Set(70,70);   //PWMʹ��FOSC��Ϊʱ��Դ����PWM���ÿ���ִֻ��һ��
	 Delay_1ms(50);
   s2=0;           //��ʼ�趨S2����
	 s3=0;           //��ʼ�趨S3����
	 t0_init();
	 UartInit();
		init_NRF24L01();        
   NRF24L01_FindMyself();         	//2401�Լ� 
	 NRF24L01_TX_Mode(0x00,0x04);             //����ģʽ
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
					if(Alert_flag==6)//ֻ�ڵ�6��·�ں� ��7��·��֮��  ��ɫ���
					{	
					 while((red_dat>green_dat))
						{ 
							 PWM_Set(100,100); //С������
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

