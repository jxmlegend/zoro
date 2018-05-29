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
//全自动下的循迹函数，含有全黑停止和全白停止检测
//返回值  0：正常全黑停止  1：遇到全白（可能是RFID或特殊地形）2：其他错误导致停车
u8 go_track( u8 speed )
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 1;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=speed;
	
	while(1) //后台中断扫描路况，调用track函数，停车后更新stop_flag值
	{
		if(Stop_Flag==0x01) break;
		if(Stop_Flag==0x04) break;
	}
	if(Stop_Flag==0x01) return 0;
	else if(Stop_Flag==0x04) return 1;
	else return 2;
}
//全自动下的码盘循迹函数，含有全黑停止和全白停止检测，超距检测
//返回值  0：正常全黑停止  1：遇到全白（可能是RFID或特殊地形）2：达到距离停车
u8 go_track_mp( u8 speed ,u16 mp_value)
{
	Roadway_mp_syn();	//码盘同步
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 1;
	Car_Spend=speed;
	temp_MP=mp_value;
	while(1) //后台中断扫描路况，调用track函数，停车后更新stop_flag值
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
//全自动下的前进函数
//u8 speed:速度   u16 mp_value：前进码盘值
void go(u8 speed ,u16 mp_value)
{
	
	Roadway_mp_syn();	//码盘同步
	Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;	Track_mp_Flag = 0;
	Car_Spend=speed;
	temp_MP=mp_value;
	Control(Car_Spend ,Car_Spend);
	while(Stop_Flag!=0x03);	
}
//全自动下的后退函数
//u8 speed:速度   u16 mp_value：前进码盘值
void back(u8 speed ,u16 mp_value)
{
	
	Roadway_mp_syn();	//码盘同步
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 1; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;	Track_mp_Flag = 0;
	Car_Spend=speed;
	temp_MP=mp_value;
	Control(-Car_Spend ,-Car_Spend);
	while(Stop_Flag!=0x03);	
}

//全自动下的缓慢前进并读卡函数，RFID卡数据在RFID_RX_buff[]
//读取完后停车
void go_readcard(void)
{
	Roadway_mp_syn();	//码盘同步
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

//全自动下左转弯函数，转弯90度，车身中心遇到垂直的循迹线停车
void turn_left(u8 speed)
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=speed;
	Control(-Car_Spend,Car_Spend);
	while(Stop_Flag!=0X02);	
}
//全自动下右转弯函数，转弯90度，车身中心遇到垂直的循迹线停车
void turn_right(u8 speed)
{
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend= speed;
	Control(Car_Spend,-Car_Spend);
	while(Stop_Flag!=0X02);	
}
//全自动下延时左转弯函数，转弯角度受time控制，停车，time=1200，大约45度
void turn_left_delay(u16 time)
{
	set_wheel_time=time;
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=1;delay_Right_flag=0;Track_mp_Flag = 0;
	Car_Spend=80;
	Control(-Car_Spend,Car_Spend);
	while(Stop_Flag!=25);
	
}

//全自动下延时右转弯函数，转弯角度受time控制，停车，time=1200，大约45度
void turn_right_delay(u16 time)
{
	set_wheel_time=time;
	Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
	Back_Flag = 0; Track_Flag = 0;delay_Left_flag=0;delay_Right_flag=1;Track_mp_Flag = 0;
	Car_Spend=80;
	Control(Car_Spend,-Car_Spend);
	while(Stop_Flag!=26);
}


//读取当前档位调节灯光

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
		setLevel=4-Current_level+level;//需要进行设置的档位
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
