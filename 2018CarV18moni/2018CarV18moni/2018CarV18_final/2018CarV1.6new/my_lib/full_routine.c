#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "canP_HostCom.h"
#include "delay.h"
#include "data_channel.h"
#include "roadway_check.h"

#define __CAN_USER_C__
#include "can_user.h"
#include "data_base.h"
#include "infrared.h"
#include "Timer.h"
#include "full_routine.h"
#include "rc522.h"
#include "tba.h"
#include "bh1750.h"
extern u8 FIND_OK;
extern u8 RFID_RX_buff[17];
extern u16 set_wheel_time;

extern uint8_t  delay_Left_flag;
extern uint8_t  delay_Right_flag;	
//ȫ�Զ��µ�ѭ������������ȫ��ֹͣ��ȫ��ֹͣ���
//����ֵ  0������ȫ��ֹͣ  1������ȫ�ף�������RFID��������Σ�2������������ͣ��
u8 go_track( u8 speed )
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 1;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=speed;
	
	while(1) //��̨�ж�ɨ��·��������track������ͣ�������stop_flagֵ
	{
		if(Stop_Flag==0x01) break;
		if(Stop_Flag==0x04) break;
	}
	if(Stop_Flag==0x01) return 0;
	else if(Stop_Flag==0x04) return 1;
	else return 2;
}
//ȫ�Զ��µ�����ѭ������������ȫ��ֹͣ��ȫ��ֹͣ��⣬������
//����ֵ  0������ȫ��ֹͣ  1������ȫ�ף�������RFID��������Σ�2���ﵽ����ͣ��
u8 go_track_mp( u8 speed ,u16 mp_value)
{
	Roadway_mp_syn();	//����ͬ��
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 1;
	Car_Spend=speed;
	temp_MP=mp_value;
	while(1) //��̨�ж�ɨ��·��������track������ͣ�������stop_flagֵ
	{
		if(Stop_Flag==0x01) break;
		if(Stop_Flag==0x04) break;
		if(Stop_Flag==0x0f) break;
	}
	if(Stop_Flag==0x01) return 0;
	else if(Stop_Flag==0x04) return 1;
	else if(Stop_Flag==0x0f) return 2;
	else return 3;
}
//ȫ�Զ��µ�ǰ������
//u8 speed:�ٶ�   u16 mp_value��ǰ������ֵ
void go(u8 speed ,u16 mp_value)
{
	
	Roadway_mp_syn();	//����ͬ��
	Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;	Track_mp_Flag = 0;
	Car_Spend=speed;
	temp_MP=mp_value;
	Control(Car_Spend ,Car_Spend);
	while(Stop_Flag!=0x03);	
}
//ȫ�Զ��µĺ��˺���
//u8 speed:�ٶ�   u16 mp_value��ǰ������ֵ
void back(u8 speed ,u16 mp_value)
{
	
	Roadway_mp_syn();	//����ͬ��
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 1; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;	Track_mp_Flag = 0;
	Car_Spend=speed;
	temp_MP=mp_value;
	Control(-Car_Spend ,-Car_Spend);
	while(Stop_Flag!=0x03);	
}

//ȫ�Զ��µĻ���ǰ��������������RFID��������RFID_RX_buff[]
//��ȡ���ͣ��
void go_readcard(void)
{
	Roadway_mp_syn();	//����ͬ��
	Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=13;
	temp_MP=400;
	Control(Car_Spend ,Car_Spend);
	delay_ms(500);
	while(Stop_Flag!=3)
	{
		Control(0,0);
		delay_ms(500);delay_ms(500);
		if	(RC522_Search_Card()) 
		{
				 FIND_OK=1;		
         Control(0,0);			
				 break;
		}	
		Control(16,16);
		delay_ms(200);
	}		
 	if(FIND_OK)
	{			
		FIND_OK=0;
		delay_ms(100);
		RW_Card(1,10, RFID_RX_buff);
		delay_ms(100);
		RW_Card(1,10, RFID_RX_buff);
		Stop_Flag = 3 ;
	}
}

//ȫ�Զ�����ת�亯����ת��90�ȣ���������������ֱ��ѭ����ͣ��
void turn_left(u8 speed)
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=speed;
	Control(-Car_Spend,Car_Spend);
	while(Stop_Flag!=0X02);	
}
//ȫ�Զ�����ת�亯����ת��90�ȣ���������������ֱ��ѭ����ͣ��
void turn_right(u8 speed)
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend= speed;
	Control(Car_Spend,-Car_Spend);
	while(Stop_Flag!=0X02);	
}
//ȫ�Զ�����ʱ��ת�亯����ת��Ƕ���time���ƣ�ͣ����time=1200����Լ45��
void turn_left_delay(u16 time)
{
	set_wheel_time=time;
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=1;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=80;
	Control(-Car_Spend,Car_Spend);
	while(Stop_Flag!=25);
	
}

//ȫ�Զ�����ʱ��ת�亯����ת��Ƕ���time���ƣ�ͣ����time=1200����Լ45��
void turn_right_delay(u16 time)
{
	set_wheel_time=time;
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=1;Track_mp_Flag = 0;
	Car_Spend=80;
	Control(Car_Spend,-Car_Spend);
	while(Stop_Flag!=26);
}


//��ȡ��ǰ��λ���ڵƹ�

void Set_level(u8 level)
{
	u8 i;
	u8 Current_level;
	u8 setLevel;
	u32 count;
	for(i=0;i<5;i++)
	{
		count+=Get_Bh_Value();
		delay_ms(100);
	}
	count/=5;
	
	if(count<500){Current_level=1;}
	else if(count<800){Current_level=2;}
	else if(count<1200){Current_level=3;}
	else if(count>1200){Current_level=4;}	
	
	if(Current_level<=level)
	{
		setLevel=level-Current_level;
	}
	else if(Current_level>level)
	{
		setLevel=4-Current_level+level;//��Ҫ�������õĵ�λ
	}
	
	
	switch(setLevel)
	{
		case 1:
			Infrared_Send(H_1,4);
			delay_ms(20);			
			break;
		case 2:
			Infrared_Send(H_2,4);
			delay_ms(20);			
			break;
		case 3:
			Infrared_Send(H_3,4);
			delay_ms(20);			
			break;
		
		
		default:	
			break;
	}
}


u8 mode(void)
{
	u8 i;
	u8 Current_level;
	u16 count;
	
	count=Get_Bh_Value();
	
	if(count<500){Current_level=1;}
	else if(count<800){Current_level=2;}
	else if(count<1200){Current_level=3;}
	else if(count>1200){Current_level=4;}	
	
	return Current_level;
}
