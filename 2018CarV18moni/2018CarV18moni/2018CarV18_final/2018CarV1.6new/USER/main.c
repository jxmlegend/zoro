/**
工程名称：2018主车综合程序			
修改时间：2018.05.08
*/
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "infrared.h"
#include "cba.h"
#include "ultrasonic.h"
#include "canp_hostcom.h"
#include "hard_can.h"
#include "bh1750.h"
#include "syn7318.h"
#include "power_check.h"
#include "can_user.h"
#include "data_base.h"
#include "roadway_check.h"
#include "tba.h"
#include "data_base.h"
#include "swopt_drv.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "delay.h"
#include "can_user.h"
#include "Timer.h"
#include "rc522.h"
#include "full_routine.h"

RCC_ClocksTypeDef RCC_Clocks;

uint16_t main_cont;
u8 RFID_RX_buff[17];
u8 RFID_TX_buff[17];
uint8_t premise_flag = 0;//寻到全白
uint8_t  rfid_return_premit=0;//RFID标志
uint8_t  delay_Left_flag_old=0;
uint8_t  delay_Left_flag=0;//延时左转
uint8_t  delay_Right_flag_old=0;
uint8_t  delay_Right_flag=0;//延时右转
uint8_t	 QR_flag=0;//二维码标志

extern u8 follower_Buff[50];//从车二维码数据
extern uint8_t Track_buf[2];
extern u8 Full_Enable;
extern u8 Mark;
extern u16 Follower_dis;
//extern u8 LED_Tx_Buff[8];
//u8 _tx_Buff[8]={0x55,0x02,0x80,0x01,0x00,0x00,0x81,0xbb};

u8 _display[]={0xFF,0x20,0x4e,0x34,0x30,0x30};//立体显示物
u8 _display1[]={0xFF,0x10,0x59,0x36,0x41,0x33};//立体显示物 //N400Y6 A3

uint16_t Light_Value = 0;				//光强度值	
u8 TEMP_FIND_OK=0;
/**
函数功能：硬件初始化
参    数：无
返 回 值：无
*/
void Hardware_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);					//中断分组

	delay_init(168);
	
	Tba_Init();														//任务板初始化
	Infrared_Init();												//红外初始化
	Cba_Init();														//核心板初始化
	Ultrasonic_Init();												//超声波初始化
	Hard_Can_Init();												//CAN总线初始化
	BH1750_Configure();												//BH1750初始化配置
	SYN7318_Init();													//语音识别初始化
	Electricity_Init();												//电量检测初始化

	UartA72_Init();
	Can_check_Init(83,999);											//CAN总线定时器初始化
	roadway_check_TimInit(83,99);									//路况检测
	Timer_Init(83,999);												//串行数据通讯时间帧
}
/**
函数功能：按键检测
参    数：无
返 回 值：无
*/
void KEY_Check()
{
	if(S1 == 0)
	{
		delay_ms(10);
		if(S1 == 0)
		{
			LED1 = !LED1;
			while(!S1);
//			Infrared_Send(HW_K,6);				//打开测试红外报警
			Infrared_Send(CP_SHOW1,6);
			delay_ms(50);
			Infrared_Send(CP_SHOW2,6);
//			Set_level(1);
		}
	}
	if(S2 == 0)
	{
		delay_ms(10);
		if(S2 == 0)
		{
			LED2 = !LED2;
			while(!S2);
//			RW_Card(1,9, RFID_RX_buff);
//			delay_ms(100);
//			Send_WifiData_To_Fifo(RFID_RX_buff,12);
//			Set_level(2);
			Full_Enable=1;Mark=4;SYN_TTS("全自动任务开始");
		}
	}
	if(S3 == 0)
	{
		delay_ms(10);
		if(S3 == 0)
		{
			LED3 = !LED3;
			while(!S3);
//			Send_InfoData_To_Fifo("1234567",7);
//			Full_Enable=1;Mark=76;
			while(1)
			{
				if(RC522_Search_Card()) 
				{
						 TEMP_FIND_OK=1;
						 break;
				}		
				Set_tba_WheelLED(L_LED,SET);
				Set_tba_WheelLED(R_LED,SET);
				delay_ms(20);
			}		
			Set_tba_WheelLED(L_LED,RESET);
			Set_tba_WheelLED(R_LED,RESET);
			if(TEMP_FIND_OK)
			{	
				TEMP_FIND_OK=0;
				delay_ms(100);
				RFID_TX_buff[0]='(';
				RFID_TX_buff[1]='\'';
				RFID_TX_buff[2]='A';
				RFID_TX_buff[3]='\'';
				RFID_TX_buff[4]='\\';
				RFID_TX_buff[5]='3';
				RFID_TX_buff[6]='/';
				RFID_TX_buff[7]='.';
				RFID_TX_buff[8]='\"';
				RFID_TX_buff[9]='E';
				RFID_TX_buff[10]='5';
				RFID_TX_buff[11]='-';
				RFID_TX_buff[12]='>';
				RFID_TX_buff[13]='B';
				RFID_TX_buff[14]=')';
				RFID_TX_buff[15]='\"';
				RW_Card(0,10,RFID_TX_buff);
			}
			
		}
	}
	
	if(S4 == 0)
	{
		delay_ms(10);
		if(S4 == 0)
		{		
			while(!S4);
			SYN7318_Test();
		}
	}
}


static uint32_t Power_check_times;		//电量检测周期
static uint32_t LED_twinkle_times;		//LED闪烁周期
static uint32_t WIFI_Upload_data_times; //通过Wifi上传数据周期
//static uint32_t read_card_times;        //读取RFID
static uint32_t Full_times;        //自动任务周期
//static uint32_t RFID_Init_Check_times;

uint32_t delay_Turn;        //延时转弯的当前时间


int main(void)
{	
	
//	uint16_t Light_Value = 0;				//光强度值	
	uint16_t CodedDisk_Value = 0;			//码盘
	uint16_t Nav_Value = 0;					//角度
	u8 level=0;
	

	
	Hardware_Init();						//硬件初始化
	
//	Set_tba_Beep(SET);
//	delay_ms(500);
//	delay_ms(500);
//	Set_tba_Beep(RESET);
	
	Readcard_daivce_Init();//RFID初始化
	
	LED_twinkle_times =  gt_get() + 50;     //
	Power_check_times =  gt_get() + 200;

	WIFI_Upload_data_times = gt_get() + 200;
//	read_card_times = gt_get() + 40;
	
	Full_times = gt_get() + 2;
	
	Principal_Tab[0] = 0x55;
	Principal_Tab[1] = 0xAA;	
	
	Follower_Tab[0] = 0x55;
	Follower_Tab[1] = 0x02;
	
	Send_UpMotor(0 ,0);
	
	
	
//	Send_ZigbeeData_To_Fifo(_tx_Buff,8);
	while(1)
	{
		KEY_Check();									//按键检测
		
		if(gt_get_sub(LED_twinkle_times) == 0) 			
		{
			LED_twinkle_times =  gt_get() + 50;			//LED4状态取反
			LED4 = !LED4;
		} 
		
		Can_WifiRx_Check();
		Can_ZigBeeRx_Check();		

		if(gt_get_sub(Full_times) == 0)
		{
			Full_times = gt_get() + 2;
			if(Full_Enable)
			{
				Full();
			}
			
		}		
		if(gt_get_sub(Power_check_times) == 0)
		{
			Power_check_times =  gt_get() + 200;
			Power_Check();								//电池电量检测
		}		
		
		if(gt_get_sub(WIFI_Upload_data_times) == 0)
		{
			WIFI_Upload_data_times =  gt_get() + 200;
		
			if(Host_AGV_Return_Flag == RESET)
			{
				
					Principal_Tab[2] = Stop_Flag;				//运行状态
					Principal_Tab[3] = Get_tba_phsis_value();	//光敏状态值返回
					
					//if(Roadway_GoBack_Check())
					Ultrasonic_Ranging();						//超声波数据						
					Principal_Tab[4]=dis%256;        
					Principal_Tab[5]=dis/256;
					
					//if(Roadway_GoBack_Check())
					Light_Value = Get_Bh_Value();				//光强度传感器	
					level=mode();
					Principal_Tab[6]=Light_Value%256;	    	//光照数据
					Principal_Tab[7]=Light_Value/256;
					
					CodedDisk_Value = CanHost_Mp;				//码盘
					Principal_Tab[8]=CodedDisk_Value%256;	    	
					Principal_Tab[9]=CodedDisk_Value/256;
					
//					Nav_Value = CanHost_Navig;					//角度
//					Principal_Tab[10]=Nav_Value%256;	    	
//					Principal_Tab[11]=Nav_Value/256;
					
					Principal_Tab[10]=level/10;	    	//光照等级
					Principal_Tab[11]=level%10;
					
//					Send_WifiData_To_Fifo(Principal_Tab,12);
//					UartA72_TxClear();
//					UartA72_TxAddStr(Principal_Tab,12);
//					UartA72_TxStart();				
        
				
			} else if((Host_AGV_Return_Flag == SET) && (AGV_data_Falg == SET)){
					UartA72_TxClear();
					UartA72_TxAddStr(Follower_Tab,50);
					UartA72_TxStart();
					//需要修改为将从车数据发送给上位机
					Send_WifiData_To_Fifo(Follower_Tab,50);
					AGV_data_Falg = 0;
//				}
				
			}
		}
	}		
}




