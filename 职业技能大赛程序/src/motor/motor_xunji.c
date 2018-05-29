#include "commom.h"
#include "delay.h"
#include "motor.h"
#include "Infrared_emission.h"
#include "STC12C5A60S2_PWM.h"
#include "motor_xunji.h"
#include "dth.h"
#include "uart.h"
#include "STC_BH1750_nicete.h"
#include "RC.h"
#include "NRF24L01.H"

/****************************************************
ȫ�ֱ�����
renwu_flag��   �������б�־λ��Ϊ1ʱ��ʾ����ִ������
Alert_flag��   �������־λ��Alert_flag��ֵ��ʾ����ִ�еڼ�������
heixian_flag�� С��ѭ����ȫ����⵽����ʱΪ0���ñ�־λ���������ڣ�
               ��ֹС������һ��·��ʱ��Alert_flag���Ӷ�Σ�Ӱ��
							 �����ִ�С�
*******************************************************/

uchar renwu_flag=0,Alert_flag=0,heixian_flag=1,Alert_flag_back=0;		
volatile uchar Alert_flag_F=0;
extern stddis;


/********************************************************
�궨�壺
        С��·��ת�䣬ʱ�����
	   ����С������״�����ɵ���ʱ��Ĵ�С
 *********************************************************/
#define  FORWARD_time  1000 //·��ת��ʱ�����
#define  LEFT_time     3000

/*
������channel ����ͨ��
�������ܣ�
�ɼ����ⵥԪ�Ĺ�ǿֵ����ʹ���ⵥԪ�Ĺ�ǿ�ȼ����ڵ�����
������ ��Ҫ���õĹ��յȼ�
*/
void ADC_capture_task(uchar Standard_Leve);
/*����ѭ������*/
void Back_FollowLine(void);
/*����������*/
void beep(uint ring_time);

/*
�������ܣ�
		С����ת��PWM�ı�
*/
void Car_turnLeft(uint time1,uint time2)
{
	  MOTOR_GO_FORWARD;	
	  Delay_1ms(time1); 
		PWM_Set(60,50);
	  MOTOR_GO_LEFT;
	  Delay_1ms(time2);
}

void Car_turnRight(uint time1,uint time2)
{
	  MOTOR_GO_FORWARD;	
	  Delay_1ms(time1); 
		PWM_Set(60,50);
	  MOTOR_GO_RIGHT;
	  Delay_1ms(time2);
}

//void Car_turnLeft(uint time1,uint time2)
//{
//	    unsigned int t=50000;
//	    volatile uchar xj_data;
//	    do
//			{ 
//				PWM_Set(70,70);
//				MOTOR_GO_FORWARD; 
//			}
//			while(t--);
//		  do{
//				  PWM_Set(65,65);
//					MOTOR_GO_LEFT ; 
//				 //xj_data=P0;
//			}
//			while(xunji<255);
//			Delay_1ms(100);
//			do{
//				  PWM_Set(65,65);
//					MOTOR_GO_LEFT ; 
//				  xj_data=P0;
//			}
//			while(xj_data==255);
//}

void RIGHT_LED_ON(void)   //����תLED�ơ�����ʾ�¶�
{

    static uchar ON_success=0;	
	
	  MOTOR_GO_FORWARD;
	  Delay_1ms(500);
	  MOTOR_GO_LEFT;
	  Delay_1ms(1500);
	  MOTOR_GO_STOP; 
	  Delay_1ms(1000);

	  if(ON_success==0)    //ֻ��һ�Σ���תLED��
		{ 
			Multiple_sendIRdata(1,0x00,0x11);//���Ϳ���תLEDָ��,����ָ��Ĵ�������ʵ����������ӻ����
			ON_success=1;
		} 
		Delay_1ms(10000);
		
		
   	Multiple_send_Temp_IRdata(4,0xf2); //���ͺ����¶�����
		
	  MOTOR_GO_RIGHT;
	  Delay_1ms(2000);
}

void FollowLine(void)
{  
	    switch(xunji)
		 {
		      case 0x1F:
					case 0x2f:
					case 0x3f:
					case 0x4f:
					case 0x5f:
					case 0x6f:
		      case 0x7F:
					case 0x8f:
					case 0x9f:
					case 0xaf:
					case 0xbf: 
					case 0xcf:						
 					case 0xdF:
			 		case 0xEF:MOTOR_GO_RIGHT; heixian_flag=1; break;		
					
					case 0xF1:
					case 0xf2:
					case 0xf3:
					case 0xf4:
					case 0xf5:
					case 0xf6:
		      case 0xF7:
					case 0xf8:
					case 0xf9:
					case 0xfa:
					case 0xfb:
					case 0xfc:						
 					case 0xFd:
			 		case 0xFe:MOTOR_GO_LEFT; heixian_flag=1; break; 		
			 
					case 0xE7:MOTOR_GO_FORWARD; heixian_flag=1; break;	   				
				  case 0xFF:/*MOTOR_GO_STOP;*/  break;		
			    case 0x00:	
									if(heixian_flag)
								 {
									MOTOR_GO_STOP; Delay_1ms(30); ++Alert_flag;renwu_flag=1;heixian_flag=0;														
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
	static u8 set_flag=1,set_flag1=1;
	if(renwu_flag)
	{		
			set_flag=1;	
			if(set_flag1==1)
			{
			  PWM_Set(59,59);/*�ӿ�ת���ٶ�*//*�ӿ�ת���ٶ�*/
				set_flag1=0;
			}
	    switch(crossroads_count)
			{
						case 1 : 				zigbee_transmint_data(0x0000,Dot_Matrix_CMD,5,6);
														delay_ms(250);delay_ms(250);
														zigbee_transmint_data(0x0000,Dot_Matrix_CMD,5,6);
														delay_ms(250);delay_ms(250);
														zigbee_transmint_data(0x0000,Dot_Matrix_CMD,5,6);
														delay_ms(250);delay_ms(250);
														Car_turnLeft(FORWARD_time,LEFT_time);//С����ת
														break;
						case 2 : 				
														Car_turnLeft(FORWARD_time,LEFT_time);												
														break;
						case 3 :        NRF24L01_transmit(Alert_led);
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;
						case 4 :   			stddis=0;
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;
						case 5 :       	NRF24L01_transmit(Light_4);//��ת��	
														MOTOR_GO_STOP;
														delay_ms(400);
														MOTOR_GO_BACK;
														delay_ms(800);
														Car_turnRight(FORWARD_time,LEFT_time);
						                MOTOR_GO_STOP;	
														stddis=1;
														break;
						case 6:         Car_turnRight(FORWARD_time,LEFT_time);
														EA=1;														//��ʱ������ʹ�� ;//���жϣ���ɫʶ����
														TR0=1;           //����
													  TR1=1;
														break;													
						case 7:        	EA=0;//���ж�
														TR0=0;           //ֹͣ��ɫʶ��
													  TR1=1;
														MOTOR_GO_FORWARD;
														Delay_1ms(200);
														break;
						case 8:       	Car_turnLeft(FORWARD_time,LEFT_time);
														MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														MOTOR_GO_LEFT;
														delay_ms(200);
						                MOTOR_GO_STOP;
														Multiple_sendIRdata(2,0x00,0x11); 
														MOTOR_GO_RIGHT;
														delay_ms(200);
						                MOTOR_GO_STOP;		 
														break;
						case 9:       	NRF24L01_transmit(ZhaDao);
														Car_turnLeft(FORWARD_time,LEFT_time);
														break;
						case 10:       	MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;	 
//													  NRF24L01_transmit(Alert_led);		
//														zigbee_control=0;
//														delay_ms(10);
//														zigbee_control=1;	
//						                Car_turnLeft(FORWARD_time,LEFT_time);//С����ת	
//						                break;
						case 11:        MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;          							       
//						                 MOTOR_GO_FORWARD;
//				                     delay_ms(100);	
//				                     Alert_flag=0;//ѭ��������·�ڱ�������
//														 break;		
						 case 12:       MOTOR_GO_STOP;
														break;
														/*����·�ڲ���С��ͣ*/						          					 					 
					 default:				  MOTOR_GO_STOP;
														break;										
			}
			renwu_flag=0;			
	}else
  {
				if(set_flag)
				{	 
					PWM_Set(70,70);/*��ֱ���ٶȽ���*/
					set_flag=0;
					set_flag1=1;
				}	 
  }		
}

/*
������channel ����ͨ��
�������ܣ�
�ɼ����ⵥԪ�Ĺ�ǿֵ����ʹ���ⵥԪ�Ĺ�ǿ�ȼ�Standard_Leve
������ ��Ҫ���õĹ��յȼ�
*/
void ADC_capture_task(uchar Standard_Leve)
{
	   uchar  i,RING_COUNT=2,RING_TIME=100;
 	   uchar  Lightleve=0;
     volatile int ADNum=0;             
		 while(Lightleve!=Standard_Leve)
		{  	
       Single_Write_BH1750(0x01);    // power on
       Single_Write_BH1750(0x10);    // H- resolution mode
       Delay180ms();                 //��ʱ180ms
			 Multiple_read_BH1750();       
       ADNum=BUF[0];
       ADNum=(ADNum*256)+BUF[1];
				if(ADNum>0&&ADNum<120)
				{	
				Lightleve=0x01;
				RING_COUNT=1;RING_TIME=700;
        }
				else if(ADNum>120&&ADNum<300)				 
				{Lightleve=0x02;
         RING_COUNT=2;RING_TIME=500;
				}
				else if(ADNum>300&&ADNum<650)
				{Lightleve=0x03;
				 RING_COUNT=3;RING_TIME=300;
        }
				else if(ADNum>650&&ADNum<2500)
				{Lightleve=0x04;
				 RING_COUNT=4;RING_TIME=100;
				}		
				if(Lightleve!=Standard_Leve)
				// Ctrl_FM_transmitting(0x03,0xb0);		
				{
								NRF24L01_transmit(Light_3);
				}					
						for(i=0;i<RING_COUNT;i++)
		           beep(RING_TIME); 
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

/*����ѭ������*/
void Back_FollowLine(void)
{ 
	 PWM_Set(70,70);/*��ֱ���ٶȽ���*/
		 switch(xunji)
		 {
		      case 0x1F:
					case 0x2f:
					case 0x3f:
					case 0x4f:
					case 0x5f:
					case 0x6f:
		      case 0x7F:
					case 0x8f:
					case 0x9f:
					case 0xaf:
					case 0xbf:
					case 0xcf:						
 					case 0xdF:
			 		case 0xEF:MOTOR_GO_RIGHT; heixian_flag=1; break;		
					
					case 0xF1:
					case 0xf2:
					case 0xf3:
					case 0xf4:
					case 0xf5:
					case 0xf6:
		      case 0xF7:
					case 0xf8:
					case 0xf9:
					case 0xfa:
					case 0xfb:
					case 0xfc:						
 					case 0xFd:
			 		case 0xFe:MOTOR_GO_LEFT; heixian_flag=1; break; 
			 	   					
					case 0xE7:MOTOR_GO_BACK; heixian_flag=1; break;	   
				  case 0xFF:MOTOR_GO_STOP;   break;		
			    case 0x00:	
									if(heixian_flag)
								 {
									 MOTOR_GO_STOP;  Alert_flag_back=1;heixian_flag=0;														
								 }	break;						 
					default:  MOTOR_GO_BACK; heixian_flag=1; break;
		 } 
}
