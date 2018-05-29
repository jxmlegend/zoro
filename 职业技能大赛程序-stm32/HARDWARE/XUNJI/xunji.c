#include  "xunji.h"
#include "motor_pwm.h"
#include "delay.h"
#include "24l01.h"
#include "RC.h"
#include "Infrared_emsiion.h"
#include "ZigBee.h"
#include "BH1750.h"
#include "i2c.h"
#include "led.h"
#include "usart.h"
#include "HCSR04.h"

extern u8 BUF[2];
extern u16 red_dat,green_dat,blue_dat;
extern u16 distance,stddis;

volatile u8 xj;



/****************************************************
ȫ�ֱ�����
renwu_flag��   �������б�־λ��Ϊ1ʱ��ʾ����ִ������
Alert_flag��   �������־λ��Alert_flag��ֵ��ʾ����ִ�еڼ�������
heixian_flag�� С��ѭ����ȫ����⵽����ʱΪ0���ñ�־λ���������ڣ�
               ��ֹС������һ��·��ʱ��Alert_flag���Ӷ�Σ�Ӱ��
							 �����ִ�С�
*******************************************************/
volatile u8 renwu_flag=0,Alert_flag=0,heixian_flag=1;	

/********************************************************
�궨�壺
        С��·��ת�䣬ʱ�����
	   ����С������״�����ɵ���ʱ��Ĵ�С
 *********************************************************/
#define  LEFT_time    500


void beep(uint ring_time);
/*
������channel ����ͨ��
�������ܣ�
�ɼ����ⵥԪ�Ĺ�ǿֵ����ʹ���ⵥԪ�Ĺ�ǿ�ȼ�Standard_Leve
������ ��Ҫ���õĹ��յȼ�
*/
void ADC_capture_task(uchar Standard_Leve);



/*
������channel ����ͨ��
�������ܣ�
�ɼ����ⵥԪ�Ĺ�ǿֵ����ʹ���ⵥԪ�Ĺ�ǿ�ȼ�Standard_Leve
������ ��Ҫ���õĹ��յȼ�
*/

void ADC_capture_task(uchar Standard_Leve)
{
	   uint  i,RING_COUNT=0,RING_TIME=100;
 	   uchar  Lightleve=0;
     volatile int ADNum=0;
		 while(Lightleve!=Standard_Leve)
		{  					
				Start_BH1750();
			 delay_ms(180);//��ʱ180ms
       Read_BH1750();
			 ADNum=(int)Convert_BH1750();	
       printf("���յȼ���%d,��ǿ��%d\n",Lightleve,ADNum);
				if(ADNum>0&&ADNum<100)
				{	 //��������1��
				Lightleve=0x01;
				RING_COUNT=1;RING_TIME=700;
        }
				else if(ADNum>99&&ADNum<200)
				 //��������2��
				{Lightleve=0x02;
         RING_COUNT=2;RING_TIME=500;
				}
				else if(ADNum>199&&ADNum<300)
				 //��������3��
				{Lightleve=0x03;
				 RING_COUNT=3;RING_TIME=300;
        }
				else if(ADNum>299)
				 //��������4��	
				{Lightleve=0x04;
				 RING_COUNT=4;RING_TIME=100;
				}	
				NRF24L01_transmit(Standard_Leve);
//						for(i=0;i<RING_COUNT;i++)
//	           beep(RING_TIME); 
	 }				 
}

/*����������*/
void beep(uint ring_time)
{
	   BEEP=0;
		 delay_ms(ring_time);
		 BEEP=1;
		 delay_ms(ring_time);
}


//ѭ��IO�ڳ�ʼ��
void XUNJI_GPIO_init(void)
{	 

	 GPIO_InitTypeDef GPIO_InitStructure;
 	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��
	 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB8-15
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB8-15
	    	
}


/*
�������ܣ�
С��ת��
С��ѭ�����⵽���ߺ�ֱ����ת
������
f_time:    С��ǰ��ʱ��
l_time:    С����תʱ��
*/

void car_wheel_left(u16 l_time)
{
	MOTOR_GO_FORWARD;
	delay_ms(204);
	MOTOR_GO_LEFT ; 
	MOTOR_GO_LEFT ; 
	delay_ms(l_time);
	MOTOR_GO_STOP;
	MOTOR_GO_STOP;
}

void car_wheel_right(u16 l_time)
{
	MOTOR_GO_FORWARD;
	delay_ms(204);
	MOTOR_GO_RIGHT ; 
	MOTOR_GO_RIGHT ; 
	delay_ms(l_time);
	MOTOR_GO_STOP;
	MOTOR_GO_STOP;
}

void  FollowLine(void)
{  
	  u8  xj1,xj2,xj3,xj4,xj5,xj6,xj7,xj0;
	  	 
	 xj0=XUNJI0;xj1=XUNJI1;xj2=XUNJI2;xj3=XUNJI3;
	 xj4=XUNJI4;xj5=XUNJI5;xj6=XUNJI6;xj7=XUNJI7;
	 xj=xj0+(xj1<<1)+(xj2<<2)+(xj3<<3)+(xj4<<4)+(xj5<<5)+(xj6<<6)+(xj7<<7);
    	
	 switch(xj)
		 {
					case 0xDF:MOTOR_GO_RIGHT; heixian_flag=1; break;  //?
					case 0xBF:MOTOR_GO_RIGHT; heixian_flag=1; break;
					case 0x8F:MOTOR_GO_RIGHT; heixian_flag=1; break; 
					case 0x9F:MOTOR_GO_RIGHT; heixian_flag=1; break;
					case 0x3F:MOTOR_GO_RIGHT;  heixian_flag=1;break;
					case 0x1F:MOTOR_GO_RIGHT;  heixian_flag=1;break;
					case 0x7f:MOTOR_GO_RIGHT; heixian_flag=1; break;
					case 0x5f:MOTOR_GO_RIGHT;heixian_flag=1;  break;
			 									
					case 0xFD:MOTOR_GO_LEFT;  heixian_flag=1; break;//?
					case 0xFB:MOTOR_GO_LEFT;  heixian_flag=1; break;
					case 0xF8:MOTOR_GO_LEFT;  heixian_flag=1; break;
					case 0xF9:MOTOR_GO_LEFT;  heixian_flag=1; break;
					case 0xF3:MOTOR_GO_LEFT;  heixian_flag=1; break; 
					case 0xF1:MOTOR_GO_LEFT;   heixian_flag=1;break;
					case 0xFE:MOTOR_GO_LEFT;   heixian_flag=1;break;
					case 0xFC:MOTOR_GO_LEFT;   heixian_flag=1;break;
			 	   					
					case 0xE7:MOTOR_GO_FORWARD; heixian_flag=1; break;	   
					case 0xF7:MOTOR_GO_FORWARD; heixian_flag=1; break; 
					case 0xEF:MOTOR_GO_FORWARD; heixian_flag=1; break;
				  case 0xFF:MOTOR_GO_STOP;   break;		
			    case 0x00:	
									if(heixian_flag)
								 {
									 MOTOR_GO_STOP;  ++Alert_flag;renwu_flag=1;heixian_flag=0;														
								 }	break;						 
					default:  MOTOR_GO_FORWARD;  heixian_flag=1;break;
		 } 
}    

/*
�������ܣ�
С��ִ��������
���ݣ�ʮ��·��crossroads_count����ֵ����������ؿ��Ƴ���
������crossroads_countʮ��·�ڵĸ���

*/
void car_task(u8 crossroads_count)
{
/*set_flag=1
  set_flag1=1;
	*/
	static u8 set_flag=1,set_flag1=1;	
	if(renwu_flag)
	{		
			set_flag=1;	
			if(set_flag1==1)
			{
				SetMotorPWM(5500,5500);/*�ӿ�ת���ٶ�*/
				set_flag1=0;
			}
	    switch(crossroads_count)
			{
						case 1 : 				zigbee_transmint_data(0x0000,Dot_Matrix_CMD,5, 6);
														car_wheel_left(LEFT_time );	
														break;
						case 2 : 				car_wheel_left(LEFT_time );												
														break;
						case 3 :        NRF24L01_transmit(Alert_led);
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;
						case 4 :        stddis=0;
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;
						case 5 :        NRF24L01_transmit(Light_4);//��ת��	
														MOTOR_GO_STOP;
														delay_ms(400);
														MOTOR_GO_BACK;
														delay_ms(800);
														car_wheel_right(LEFT_time);
						                MOTOR_GO_STOP;
														stddis=100;
														break;
						case 6 :      	car_wheel_right(LEFT_time);
														while(Alert_flag==6)
														{
															if(red_dat>green_dat)
														 {
															 MOTOR_GO_STOP;
														 }
															else	 
															{
																	FollowLine();
															}
														}
														break;
						case 7:       	MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;														
						case 8:        	car_wheel_left(LEFT_time);
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														car_wheel_left(LEFT_time);
														Multiple_sendIRdata(2,0x00,0x11);
														car_wheel_right(LEFT_time);
						                MOTOR_GO_STOP;
														break;														
						case 9:       	NRF24L01_transmit(ZhaDa);
														car_wheel_left(LEFT_time);
														break;
						case 10:       	MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;													
						case 11:       	MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;
						case 12:       	NRF24L01_transmit(Light_off);
														MOTOR_GO_STOP;
														break;								
					 default:				  MOTOR_GO_STOP;
														break;
			}
			renwu_flag=0;			
	}else
  {
		if(set_flag)
		{	 
			SetMotorPWM(5000,5000);/*��ֱ���ٶȽ���*/
			set_flag=0;
			 set_flag1=1;
		}	 
  }		
}

