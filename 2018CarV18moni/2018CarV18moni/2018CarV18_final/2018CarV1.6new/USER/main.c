/**
�������ƣ�2018�����ۺϳ���			
�޸�ʱ�䣺2018.05.08
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
uint8_t premise_flag = 0;//Ѱ��ȫ��
uint8_t  rfid_return_premit=0;//RFID��־
uint8_t  delay_Left_flag_old=0;
uint8_t  delay_Left_flag=0;//��ʱ��ת
uint8_t  delay_Right_flag_old=0;
uint8_t  delay_Right_flag=0;//��ʱ��ת
uint8_t	 QR_flag=0;//��ά���־

extern u8 follower_Buff[50];//�ӳ���ά������
extern uint8_t Track_buf[2];
extern u8 Full_Enable;
extern u8 Mark;
extern u16 Follower_dis;
//extern u8 LED_Tx_Buff[8];
//u8 _tx_Buff[8]={0x55,0x02,0x80,0x01,0x00,0x00,0x81,0xbb};

u8 _display[]={0xFF,0x20,0x4e,0x34,0x30,0x30};//������ʾ��
u8 _display1[]={0xFF,0x10,0x59,0x36,0x41,0x33};//������ʾ�� //N400Y6 A3

uint16_t Light_Value = 0;				//��ǿ��ֵ	
u8 TEMP_FIND_OK=0;
/**
�������ܣ�Ӳ����ʼ��
��    ������
�� �� ֵ����
*/
void Hardware_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);					//�жϷ���

	delay_init(168);
	
	Tba_Init();														//������ʼ��
	Infrared_Init();												//�����ʼ��
	Cba_Init();														//���İ��ʼ��
	Ultrasonic_Init();												//��������ʼ��
	Hard_Can_Init();												//CAN���߳�ʼ��
	BH1750_Configure();												//BH1750��ʼ������
	SYN7318_Init();													//����ʶ���ʼ��
	Electricity_Init();												//��������ʼ��

	UartA72_Init();
	Can_check_Init(83,999);											//CAN���߶�ʱ����ʼ��
	roadway_check_TimInit(83,99);									//·�����
	Timer_Init(83,999);												//��������ͨѶʱ��֡
}
/**
�������ܣ��������
��    ������
�� �� ֵ����
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
//			Infrared_Send(HW_K,6);				//�򿪲��Ժ��ⱨ��
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
			Full_Enable=1;Mark=4;SYN_TTS("ȫ�Զ�����ʼ");
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


static uint32_t Power_check_times;		//�����������
static uint32_t LED_twinkle_times;		//LED��˸����
static uint32_t WIFI_Upload_data_times; //ͨ��Wifi�ϴ���������
//static uint32_t read_card_times;        //��ȡRFID
static uint32_t Full_times;        //�Զ���������
//static uint32_t RFID_Init_Check_times;

uint32_t delay_Turn;        //��ʱת��ĵ�ǰʱ��


int main(void)
{	
	
//	uint16_t Light_Value = 0;				//��ǿ��ֵ	
	uint16_t CodedDisk_Value = 0;			//����
	uint16_t Nav_Value = 0;					//�Ƕ�
	u8 level=0;
	

	
	Hardware_Init();						//Ӳ����ʼ��
	
//	Set_tba_Beep(SET);
//	delay_ms(500);
//	delay_ms(500);
//	Set_tba_Beep(RESET);
	
	Readcard_daivce_Init();//RFID��ʼ��
	
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
		KEY_Check();									//�������
		
		if(gt_get_sub(LED_twinkle_times) == 0) 			
		{
			LED_twinkle_times =  gt_get() + 50;			//LED4״̬ȡ��
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
			Power_Check();								//��ص������
		}		
		
		if(gt_get_sub(WIFI_Upload_data_times) == 0)
		{
			WIFI_Upload_data_times =  gt_get() + 200;
		
			if(Host_AGV_Return_Flag == RESET)
			{
				
					Principal_Tab[2] = Stop_Flag;				//����״̬
					Principal_Tab[3] = Get_tba_phsis_value();	//����״ֵ̬����
					
					//if(Roadway_GoBack_Check())
					Ultrasonic_Ranging();						//����������						
					Principal_Tab[4]=dis%256;        
					Principal_Tab[5]=dis/256;
					
					//if(Roadway_GoBack_Check())
					Light_Value = Get_Bh_Value();				//��ǿ�ȴ�����	
					level=mode();
					Principal_Tab[6]=Light_Value%256;	    	//��������
					Principal_Tab[7]=Light_Value/256;
					
					CodedDisk_Value = CanHost_Mp;				//����
					Principal_Tab[8]=CodedDisk_Value%256;	    	
					Principal_Tab[9]=CodedDisk_Value/256;
					
//					Nav_Value = CanHost_Navig;					//�Ƕ�
//					Principal_Tab[10]=Nav_Value%256;	    	
//					Principal_Tab[11]=Nav_Value/256;
					
					Principal_Tab[10]=level/10;	    	//���յȼ�
					Principal_Tab[11]=level%10;
					
//					Send_WifiData_To_Fifo(Principal_Tab,12);
//					UartA72_TxClear();
//					UartA72_TxAddStr(Principal_Tab,12);
//					UartA72_TxStart();				
        
				
			} else if((Host_AGV_Return_Flag == SET) && (AGV_data_Falg == SET)){
					UartA72_TxClear();
					UartA72_TxAddStr(Follower_Tab,50);
					UartA72_TxStart();
					//��Ҫ�޸�Ϊ���ӳ����ݷ��͸���λ��
					Send_WifiData_To_Fifo(Follower_Tab,50);
					AGV_data_Falg = 0;
//				}
				
			}
		}
	}		
}




