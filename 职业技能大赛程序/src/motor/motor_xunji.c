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
全局变量：
renwu_flag：   任务运行标志位，为1时表示正在执行任务
Alert_flag：   任务类标志位，Alert_flag的值表示正在执行第几号任务
heixian_flag： 小车循迹板全部检测到黑线时为0，该标志位的作用在于，
               防止小车经过一个路口时，Alert_flag增加多次，影响
							 任务的执行。
*******************************************************/

uchar renwu_flag=0,Alert_flag=0,heixian_flag=1,Alert_flag_back=0;		
volatile uchar Alert_flag_F=0;
extern stddis;


/********************************************************
宏定义：
        小车路口转弯，时间控制
	   根据小车运行状况，可调节时间的大小
 *********************************************************/
#define  FORWARD_time  1000 //路口转弯时间控制
#define  LEFT_time     3000

/*
参数：channel 采样通道
函数功能：
采集调光单元的光强值，并使调光单元的光强等级处于第三级
参数： 需要设置的光照等级
*/
void ADC_capture_task(uchar Standard_Leve);
/*后退循迹函数*/
void Back_FollowLine(void);
/*蜂鸣器发声*/
void beep(uint ring_time);

/*
函数功能：
		小车左转，PWM改变
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

void RIGHT_LED_ON(void)   //开旋转LED灯、并显示温度
{

    static uchar ON_success=0;	
	
	  MOTOR_GO_FORWARD;
	  Delay_1ms(500);
	  MOTOR_GO_LEFT;
	  Delay_1ms(1500);
	  MOTOR_GO_STOP; 
	  Delay_1ms(1000);

	  if(ON_success==0)    //只开一次，旋转LED灯
		{ 
			Multiple_sendIRdata(1,0x00,0x11);//发送开旋转LED指令,发送指令的次数根据实际情况，增加或减少
			ON_success=1;
		} 
		Delay_1ms(10000);
		
		
   	Multiple_send_Temp_IRdata(4,0xf2); //发送红外温度数据
		
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
函数功能：
小车执行任务函数
根据，十字路口crossroads_count计数值，来运行相关控制程序
参数：crossroads_count十字路口的个数
*/
void car_task(u8 crossroads_count)
{
	static u8 set_flag=1,set_flag1=1;
	if(renwu_flag)
	{		
			set_flag=1;	
			if(set_flag1==1)
			{
			  PWM_Set(59,59);/*加快转弯速度*//*加快转弯速度*/
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
														Car_turnLeft(FORWARD_time,LEFT_time);//小车左转
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
						case 5 :       	NRF24L01_transmit(Light_4);//左转弯	
														MOTOR_GO_STOP;
														delay_ms(400);
														MOTOR_GO_BACK;
														delay_ms(800);
														Car_turnRight(FORWARD_time,LEFT_time);
						                MOTOR_GO_STOP;	
														stddis=1;
														break;
						case 6:         Car_turnRight(FORWARD_time,LEFT_time);
														EA=1;														//定时计数初使化 ;//开中断，颜色识别红灯
														TR0=1;           //启动
													  TR1=1;
														break;													
						case 7:        	EA=0;//关中断
														TR0=0;           //停止颜色识别
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
//						                Car_turnLeft(FORWARD_time,LEFT_time);//小车左转	
//						                break;
						case 11:        MOTOR_GO_FORWARD;
														delay_ms(200);
						                MOTOR_GO_STOP;
														break;          							       
//						                 MOTOR_GO_FORWARD;
//				                     delay_ms(100);	
//				                     Alert_flag=0;//循环结束，路口变量清零
//														 break;		
						 case 12:       MOTOR_GO_STOP;
														break;
														/*其他路口不让小车停*/						          					 					 
					 default:				  MOTOR_GO_STOP;
														break;										
			}
			renwu_flag=0;			
	}else
  {
				if(set_flag)
				{	 
					PWM_Set(70,70);/*走直线速度降慢*/
					set_flag=0;
					set_flag1=1;
				}	 
  }		
}

/*
参数：channel 采样通道
函数功能：
采集调光单元的光强值，并使调光单元的光强等级Standard_Leve
参数： 需要设置的光照等级
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
       Delay180ms();                 //延时180ms
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

/*蜂鸣器发声*/
void beep(uint ring_time)
{
	   BEEP=0;
		 delay_ms(ring_time);
		 BEEP=1;
		 delay_ms(ring_time);
}

/*后退循迹函数*/
void Back_FollowLine(void)
{ 
	 PWM_Set(70,70);/*走直线速度降慢*/
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
