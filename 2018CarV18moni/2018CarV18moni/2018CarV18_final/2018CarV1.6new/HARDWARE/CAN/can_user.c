#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "canP_HostCom.h"
#include "delay.h"
#include "data_channel.h"
#include "roadway_check.h"

#define __CAN_USER_C__
#include "can_user.h"
#include "tba.h"
#include "data_base.h"
#include "cba.h"
#include "infrared.h"
#include "syn7318.h"
#include <string.h>
#include "Timer.h"
#include "uart_a72.h"
#include "ultrasonic.h"
#include "rc522.h"
#include "full_routine.h"

#define delay_Time 1500   //5ms的倍数
u8 LED_Tx_Buff[8]={0x55,0x04,0X04,0x00,0x00,0x00,0x00,0xBB};//LED显示第二排距离 第四位第五位为数据
u8 jishi_tx_Buff[8]={0x55,0x04,0X03,0X02,0X00,0X00,0X05,0XBB};//计时清零
u8 jishiopen_tx_Buff[8]={0x55,0x04,0X03,0X01,0X00,0X00,0X04,0XBB};//计时打开
u8 jishioff_tx_Buff[8]={0x55,0x04,0X03,0X00,0X00,0X00,0X03,0XBB};//计时打开

u8 garage_Tx_Buff[8]={0x55,0x0D,0X01,0x00,0x00,0x00,0x00,0xBB};//车库控制指令

u8 yuyin_tx_Buff[13]={0xFD,0x00,0x0A,0x01,0x01,0xB4,0xF2,0xBF,0xAA,0xD3,0xEF,0xD2,0xF4};//播报指定语音

u8 daoza_tx_Buff[8]={0x55,0x03,0x01,0x01,0x00,0x00,0x02,0xBB};//道闸打开

u8 agvflag_tx_Buff[8]={0x55,0x02,0x80,0x01,0x00,0x00,0x81,0xbb};//上传从车状态
u8 agvflagoff_tx_Buff[8]={0x55,0x02,0x80,0x00,0x00,0x00,0x80,0xbb};//关闭上传从车状态

u8 agv_tx_Buff[8]={0X55,0X02,0X06,0X32,0X00,0X00,0X38,0XBB};  //agv循迹
u8 agvqianjin_tx_Buff[8]={0X55,0X02,0X02,0X60,0X90,0X01,0Xf3,0XBB};  //agv前进   码盘值400
u8 agvqianjin1500_tx_Buff[8]={0X55,0X02,0X02,0X60,0Xdc,0X05,0X43,0XBB};  //agv前进1500	60是96  5dc是1500
u8 agvright_tx_Buff[8]={0X55,0X02,0X05,0X60,0X00,0X00,0X65,0XBB};	//agv右转命令 96速度  发的是十六进制
u8 agvleft_tx_Buff[8]={0X55,0X02,0X04,0X60,0X00,0X00,0X64,0XBB};	//agv右转命令 96速度	
u8 agvqianjinMP_tx_Buff[]={0X55,0X02,0X02,0X60,0X58,0X02,0Xbc,0XBB};  //agv前进1500	60是96  258是600  进车库

u8 WUXIAN_tx_Buff[8]={0X55,0X0A,0X01,0X01,0X00,0X00,0X02,0XBB};//无线充电
u8 ZHUflag_tx_Buff[8]={0x55,0xAA,0x80,0x01,0x00,0x00,0x81,0xbb};//主车状态上传
u8 AGV_START_READ_QR[]={0X55,0X02,0X92,0X01,0X00,0X00,0X93,0XBB};//开始识别二维码
u8 IR1_TX_BUFF[]={0x03,0x05,0x14,0x45,0xde,0x92};    //报警台
u8 ludeng_TX_BUFF[]={0X55,0XAA,0X63,0X00,0X00,0X00,0X63,0XBB};  //路灯
u8 RFID_DATA[30]={0X55,0XAA}; //RFID卡原始数据 

u8 Traffic_LED_Buf[8]={0X55, 0X0E, 0X01, 0X00, 0X00, 0X00, 0X01, 0XBB}; //交通灯进入识别模式
u8 Traffic_verify_Buf[8]={0X55, 0X0E, 0X02, 0X01, 0X00, 0X00, 0X03, 0XBB}; //交通灯识别结果请求确认  红绿黄  默认红

u8 TFT_buf[]={0X55 ,0X0b, 0X10, 0X00, 0X01, 0X00, 0X00, 0XBB};//TFT显示某个图片




uint8_t Wifi_Rx_Buf[ WIFI_MAX_NUM ];
uint8_t Zigb_Rx_Buf[ ZIGB_RX_MAX ];
uint8_t Wifi_Rx_num ;
uint8_t Wifi_Rx_flag ;        //wifi接收完成标志位
uint8_t Zigbee_Rx_num ;
uint8_t Zigbee_Rx_flag ;      //zigbee接收完成标志位

uint8_t Host_AGV_Return_Flag = RESET;
uint8_t AGV_data_Falg = RESET;

uint8_t AGV_CMD_Sent=0;

uint32_t canu_wifi_rxtime = 0;
uint32_t canu_zibe_rxtime = 0;

u8 Wifi_Rx_Buf_Temp[17];//wifi转ASCII码数组
u8 follower_Buff[50];//从车二维码数据
u8 Full_Enable=0;//全自动使能
u8 Mark;//任务数量
u8 FIND_OK=0;//寻到RFIF卡标志
u8 RFID_READED=0;//
u16 FULL_MP=0;//全自动行驶码盘值
u16 Follower_dis=0;//从车超声波测距数据
extern uint8_t  rfid_return_premit;
extern uint8_t  delay_Left_flag;
extern uint8_t  delay_Right_flag;
extern uint32_t delay_Turn;
extern uint8_t	QR_flag;      //Android请求主车返回从车识别的二维码标志
extern u8 RFID_RX_buff[17];
extern uint8_t Track_buf[2];

//请求解密数据格式：55 02 92 01 1E 3C 33 2C 34 2C 35 2C 37 2C 38 2C 39 2C 31 31 3E 2F 3C 31 2C 32 2C 33 2C 34 2C 35 2C 36 3E BB 00 00 00 00 00 00 00 00 00 00 00 00 00
uint8_t QR_PROCESSED_DATA[20];       //从车二维码解密后的数据:             66 AA 93 ** ** ** ** ** ** ** ** BB     (不超过20字节)

//请求识别解密主车二维码数据格式：0x    55 AA A0 01 00 00 00 BB
uint8_t MAIN_QR_DATA[]={  0x55 ,0xAA, 0xA0, 0x01, 0x00,0x00, 0xA1, 0xBB};                                  
uint8_t MAIN_QR_PROCESSED_DATA[20];  //主车二维码解密后的数据:             66 AA A0 len ** ** ** ** ** ** ** ** BB (不超过20字节)

//RFID请求解密数据格式：
//55 AA 88 09 48 45 4C 4C 4F 5F 4D 52 
//55 AA 88 10 43 35 32 32 2E 2E 2E 2E 
uint8_t RFID_PROCESSED_DATA[20];   //RFID解密后的数据               66 AA 88 len ** ** ** ** ** ** ** ** BB(不超过20字节)
//主车请求android识别车牌格式：  55 AA 94 01 00 00 00 00 00 00 00 00 
uint8_t LICENSE_DATA[]={0x55 ,0xAA ,0x94 ,0x01 ,0x00 ,0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00};             
uint8_t LICENSE_PROCESSED_DATA[20];//车牌识别处理后的数据           66 AA 94 len ** ** ** ** ** ** ** ** BB(不超过20字节)
 

//主车请求android识别图形格式：  55 AA 96 01 00 00 00 00 00 00 00 00 
uint8_t GRAPHIC_DATA[]={0x55 ,0xAA ,0x96 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 };                                  
uint8_t GRAPHIC_PROCESSED_DATA[20];//图形识别处理后的数据           66 AA 96 len (A)(0*) (B) (0*) (C) (0*) (D) (0*) (E) (0*) BB (不超过20字节)

 //主车请求android识别交通灯格式：55 AA 98 01 00 00 99 BB
uint8_t TRAFFIC_LIGHT_DATA[]= {0x55, 0xAA ,0x98 ,0x01 ,0x00 ,0x00 ,0x99 ,0xBB};  
uint8_t TRAFFIC_LIGHT_PROCESSED_DATA[20];//交通灯识别处理后的数据   66 AA 98 len 1/2/3(红/黄/绿) ** ** ** ** ** ** ** BB  (不超过20字节)

u8  RFID_PROCESSED=0;    //RFID解密完成标志
u8  QR_PROCESSED=0;      //从车二维码解密完成标志
u8  MAIN_QR_PROCESSED=0; //主车二维码识别解密完成标志
u8  LICENSE_PROCESSED=0; //车牌识别解密完成标志
u8  GRAPHIC_PROCESSED=0; //图像识别解密完成标志
u8  TRAFFIC_LIGHT_PROCESSED=0; //交通灯识别解密完成标志
u8  GARAGE_NO;//从解密数据中取得车库层数
u8  AGV_FX;//解密数据中取得从车方向
u8	AGV_CK_VAL;//AGV小车车库

void hex2ascii(u8 *in_array,u8 *out_array)
{
	u8 i;
	u8 a[12];
	a[0]=*in_array>>4;a[1]=(*in_array)&0x0f;in_array++;
	a[2]=*in_array>>4;a[3]=(*in_array)&0x0f;in_array++;
	a[4]=*in_array>>4;a[5]=(*in_array)&0x0f;//in_array++;	
	//a[6]=*in_array>>4;a[7]=(*in_array)&0x0f;	
	for(i=0;i<6;i++)
	{
		if((a[i]>=0)&&(a[i]<=9))
			a[i]+=48;
		else if((a[i]>=10)&&(a[i]<=15))
			a[i]+=55;	
	}
  for(i=0;i<6;i++)
	{
		*out_array=a[i];
		out_array++;
	}
}


void Can_WifiRx_Save(uint8_t res)           
{
	if(Wifi_Rx_flag == 0)
	{
		canu_wifi_rxtime = gt_get()+10;
		Wifi_Rx_num =0;
		Wifi_Rx_Buf[Wifi_Rx_num]=res;
		Wifi_Rx_flag = 1;
	}
	else if(Wifi_Rx_num < WIFI_MAX_NUM )	
	{
		Wifi_Rx_Buf[++Wifi_Rx_num]=res;	 
	}
}



uint8_t Rx_Flag ;

void Normal_data(void)	  // 正常接收8字节控制指令
{
	u8 sum=0;

	if(Wifi_Rx_Buf[7]==0xbb)	 // 判断包尾
	 {									  
		//主指令与三位副指令左求和校验
		//注意：在求和溢出时应该对和做256取余。
		 sum=(Wifi_Rx_Buf[2]+Wifi_Rx_Buf[3]+Wifi_Rx_Buf[4]+Wifi_Rx_Buf[5])%256;
		if(sum == Wifi_Rx_Buf[6])
		{
			   Rx_Flag =1;
		}
		else Rx_Flag =0;
	}
}

void Abnormal_data(void)	  //数据异常处理
{
	u8 i,j;
	u8 sum=0;
	
	if(Wifi_Rx_num <8)			// 异常数据字节数小于8字节不做处理
	{
	   Rx_Flag =0;
	}
	else {
		for(i=0;i<=(Wifi_Rx_num -7);i++)  
		{
			if(Wifi_Rx_Buf[i]==0x55)	   // 寻找包头
			{
			   if(Wifi_Rx_Buf[i+7]==0xbb)	// 判断包尾
			   {
			   	    sum=(Wifi_Rx_Buf[i+2]+Wifi_Rx_Buf[i+3]+Wifi_Rx_Buf[i+4]+Wifi_Rx_Buf[i+5])%256;

		           if(sum==Wifi_Rx_Buf[i+6])	 // 判断求和
	              {
			         for(j=0;j<8;j++)
					 {
					 	 Wifi_Rx_Buf[j]=Wifi_Rx_Buf[j+i];	 // 数据搬移
					 }
					    Rx_Flag =1;
		          }
	  	         else Rx_Flag =0;
		       }
			}
	    }

	}
}

uint8_t Infrared_Tab[6];			//红外数据存放数组

void Can_WifiRx_Check(void)
{
	if(Wifi_Rx_flag)
	{	
		if(gt_get_sub(canu_wifi_rxtime) == 0)
		{
			if(Wifi_Rx_Buf[0]==0xFD)  
			{			
				Send_ZigbeeData_To_Fifo( Wifi_Rx_Buf , (Wifi_Rx_num +1)); 
			}
			else if(Wifi_Rx_Buf[0]==0x66) //Android返回的解密信息格式
			{	
				if(Wifi_Rx_Buf[1]==0xAA)
				{					
					switch(Wifi_Rx_Buf[2])
					{
						case 0x88:  //ANDROID返回RFID卡解密数据
							strncpy(RFID_PROCESSED_DATA,Wifi_Rx_Buf,20);       RFID_PROCESSED=1;  //表示的RFID解密完成
							break;
						case 0x93:  //ANDROID返回从车二维码解密数据  
							strncpy(QR_PROCESSED_DATA,Wifi_Rx_Buf,20);         QR_PROCESSED=1;
							break;
						case 0x94:  //ANDROID返回主车车牌解密数据
							strncpy(LICENSE_PROCESSED_DATA,Wifi_Rx_Buf,20);     LICENSE_PROCESSED=1;
							break;
						case 0x96:  //ANDROID返回图形解密数据  
							strncpy(GRAPHIC_PROCESSED_DATA,Wifi_Rx_Buf,20);     GRAPHIC_PROCESSED=1;
							break; 
						case 0x98:  //ANDROID返回交通灯解密数据  
							strncpy(TRAFFIC_LIGHT_PROCESSED_DATA,Wifi_Rx_Buf,20);TRAFFIC_LIGHT_PROCESSED=1;
						break;  
						case 0xA0:  //ANDROID返回主车二维码解密数据  
							strncpy(MAIN_QR_PROCESSED_DATA,Wifi_Rx_Buf,20);      MAIN_QR_PROCESSED=1;
						break;
						default: break;
					}
			 }
			}
			else if(Wifi_Rx_Buf[0]==0x55)  //Android下发的控制指令
			{              
				Normal_data();  
			}
			else
			{
				Abnormal_data();      
			} 	
			Wifi_Rx_flag = 0;
		}
	}
	if(Rx_Flag ==1)	
	{    
		hex2ascii(Wifi_Rx_Buf,Wifi_Rx_Buf_Temp);
	  Wifi_Rx_Buf_Temp[6]='\n';
//		Wifi_Rx_Buf_Temp[0]=(Wifi_Rx_Buf[2]>>4)+0x30;
//		Wifi_Rx_Buf_Temp[1]=(Wifi_Rx_Buf[2]&0x0f)+0x30;
//		Wifi_Rx_Buf_Temp[2]='\n';
		Send_Debug_Info(Wifi_Rx_Buf_Temp,7);
		
//		Send_WifiData_To_Fifo(Wifi_Rx_Buf,8);
		if(Wifi_Rx_Buf[1] == 0xAA) 	   
		{	
			switch(Wifi_Rx_Buf[2])
			{
			case 0x01:              	//停止
				Send_UpMotor(0 ,0);
				Roadway_Flag_clean();	//清除标志位状态	
				break;
			case 0x02:              //前进
				Roadway_mp_syn();	//码盘同步
				Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				Control(Car_Spend ,Car_Spend);
				break;
			case 0x03:              //后退
				Roadway_mp_syn();	//码盘同步
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 1; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Control(-Car_Spend ,-Car_Spend);					
				break;
			case 0x04:              //左转
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];				
				Control(-Car_Spend ,Car_Spend);
				break;
			case 0x05:              //右转
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];
				Control(Car_Spend,-Car_Spend);
				break;
			case 0x06:              //循迹
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 1;
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				break;
			case 0x07:				//码盘清零
				
				break;
			case 0x08:				//左转弯--角度											
				Roadway_nav_syn();	//角度同步
				Roadway_mp_syn();	//码盘同步
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];				
				Send_UpMotor(-Car_Spend ,Car_Spend);					
				break;
			case 0x09:				//右转弯--角度			
				Roadway_nav_syn();	//角度同步
				Roadway_mp_syn();	//码盘同步
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Send_UpMotor(Car_Spend,-Car_Spend);
				break;
			case 0x10:										//红外前三位数据
				Infrared_Tab[0]=Wifi_Rx_Buf[3];
				Infrared_Tab[1]=Wifi_Rx_Buf[4];
				Infrared_Tab[2]=Wifi_Rx_Buf[5];
				break;
			case 0x11:										//红外后三位数据
				Infrared_Tab[3]=Wifi_Rx_Buf[3];//数据第四位
				Infrared_Tab[4]=Wifi_Rx_Buf[4];//低位校验码
				Infrared_Tab[5]=Wifi_Rx_Buf[5];//高位校验码
				break;
			case 0x12:										//通知小车单片机发送红外线
				Infrared_Send(Infrared_Tab,6);
				
				break;									
			case 0x20:	//转向灯控制
				Set_tba_WheelLED(L_LED,Wifi_Rx_Buf[3]);
				Set_tba_WheelLED(R_LED,Wifi_Rx_Buf[4]);
				break;					
			case 0x30:
				Set_tba_Beep(Wifi_Rx_Buf[3]);				//蜂鸣器
				break;
			case 0x40:										//暂未使用
				
				break;
				case 0x50:  				//红外发射控制相片上翻 
				Infrared_Send(H_S,4);
				break;
			case 0x51:    					//红外发射控制相片下翻 
				Infrared_Send(H_X,4);
				break;
			case 0x61:    					//红外发射控制光源强度档位加1 
				Infrared_Send(H_1,4);
				Send_InfoData_To_Fifo("1\n",3);
				break;
			case 0x62:   	 				//红外发射控制光源强度档位加2 
				Infrared_Send(H_2,4);
				Send_InfoData_To_Fifo("2\n",3);
				break;
			case 0x63:    					//红外发射控制光源强度档位加3 
				Infrared_Send(H_3,4);
				Send_InfoData_To_Fifo("3\n",3);
				break;
		   case 0x80:						//运动标志物数据返回允许位
			    Host_AGV_Return_Flag = Wifi_Rx_Buf[3];   //SET 允许 / RESET 禁止
				break;
//			 case 0x88: rfid_return_premit=1; //主机读取RFID命令
//        break;
			 case 0x14: delay_Left_flag=1; delay_Turn = gt_get() +delay_Time;//获取当前时间//主机发送延时左转命令
        break;
			 case 0x15: delay_Right_flag=1; delay_Turn = gt_get() +delay_Time;//获取当前时间//主机发送延时右转命令
        break;
			 case 0x92:            //ANDROID请求主车返回从车识别的二维码数据命令
					QR_flag=1;		
					UartA72_TxClear();
					UartA72_TxAddStr(follower_Buff,50);
					UartA72_TxStart();
			    Send_WifiData_To_Fifo(follower_Buff,50);
        break;
			
			 case 0x76:
				 Send_ZigbeeData_To_Fifo(AGV_CP_SHOW1,8);
					delay_ms(30);
					Send_ZigbeeData_To_Fifo(AGV_CP_SHOW2,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_CP_SHOW3,8);
				break;
			 
			 case 77:
				  Send_ZigbeeData_To_Fifo(AGV_CP_SHOW4,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_CP_SHOW5,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_CP_SHOW6,8);
				 break;
			 
			 case 78://从车控制显示立体显示距离
				  Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW1,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW2,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW3,8);
				 break;
			 
			 case 0xFF: Full_Enable=1;Mark=4;SYN_TTS("全自动任务开始");//获取当前时间//主机发送延时右转命令 
        break;
		   default:
			   break;
			}
		}
		else  
		{
			Send_ZigbeeData_To_Fifo(Wifi_Rx_Buf,8); 
		}
		Rx_Flag = 0;
	}
}

void Full(void)
{

//	u8 check_sum;	
	switch(Mark)
	{
		//stop_flag:  01十字路口 02转弯 03前进后退 04全白 0F 循迹距离达到设定值
		
			case 4:
			Send_ZigbeeData_To_Fifo(jishi_tx_Buff,8);//计时清零
			delay_ms(20);
			Mark=5;
			break;
		
		case 5:
			Send_ZigbeeData_To_Fifo(SMG_JSK,8);//计时开始
			delay_ms(20);
		  FULL_MP=CanHost_Mp;
			Mark=6;
		  break;
		
		case 6://寻迹		
			go_track(50);
			Mark=7;
			break;
		
		case 7://前进
			go(50,300);
			Mark=10;
			break;
		
		case 10://寻迹距离到主车识别二维码处
		  if(go_track_mp(30,1050)==1)
			{
				if((CanHost_Mp-FULL_MP)<1200) //在向D2前进过程中半路遇见RFID卡
					Mark=12; //发送主车识别二维码要求				
//				SYN_TTS("遇到卡");
			}
			else Mark=20;	//途中没遇到卡
			break;			
		
		case 12:			
      go_readcard();
//		  SYN_TTS("读卡完毕");
		  Mark=13;
			break;	
		
		case 13://发送RFID数据
			
			RFID_DATA[2]=0x88;
			RFID_DATA[3]=0x09;
			RFID_DATA[4]=RFID_RX_buff[0];
			RFID_DATA[5]=RFID_RX_buff[1];
			RFID_DATA[6]=RFID_RX_buff[2];
			RFID_DATA[7]=RFID_RX_buff[3];
			RFID_DATA[8]=RFID_RX_buff[4];
			RFID_DATA[9]=RFID_RX_buff[5];
			RFID_DATA[10]=RFID_RX_buff[6];
			RFID_DATA[11]=RFID_RX_buff[7];
//			Send_WifiData_To_Fifo(RFID_DATA,12);
//			UartA72_TxClear();
//			UartA72_TxAddStr(RFID_DATA,12);
//			UartA72_TxStart();
//			delay_ms(20);
//		
//			RFID_DATA[2]=0x88;
//			RFID_DATA[3]=0x10;
			RFID_DATA[12]=RFID_RX_buff[8];
			RFID_DATA[13]=RFID_RX_buff[9];
			RFID_DATA[14]=RFID_RX_buff[10];
			RFID_DATA[15]=RFID_RX_buff[11];
			RFID_DATA[16]=RFID_RX_buff[12];
			RFID_DATA[17]=RFID_RX_buff[13];
			RFID_DATA[18]=RFID_RX_buff[14];
			RFID_DATA[19]=RFID_RX_buff[15];
			Send_WifiData_To_Fifo(RFID_DATA,20);
			UartA72_TxClear();
			UartA72_TxAddStr(RFID_DATA,20);
			UartA72_TxStart();
			
//			SYN_TTS("上传射频卡数据完毕");
			Mark=14;
			break;		
			
		case 14 : //等待RFID解密完成			
			if(!RFID_PROCESSED)
			{
			    Set_tba_WheelLED(L_LED,SET);
				  Set_tba_WheelLED(R_LED,SET);
				  Mark=14;
			}	
			else
			{
				Set_tba_WheelLED(L_LED,RESET);
				Set_tba_WheelLED(R_LED,RESET);
				AGV_FX=	RFID_PROCESSED_DATA[4];	
				if(AGV_FX==1) ; //SYN_TTS("车头方向为一");
				else if(AGV_FX==2);  //SYN_TTS("车头方向为二");
				else if(AGV_FX==3)  ;//SYN_TTS("车头方向为三");
				else if(AGV_FX==4) ; //SYN_TTS("车头方向为四");
				else ;//SYN_TTS("解密错误");
				Mark=20;
			}			
			break;
			
		case 20://请求解密主车二维码
			Send_WifiData_To_Fifo(MAIN_QR_DATA,12);
//			SYN_TTS("请求解密主车二维码");
			Mark=25;
			break;
			
		case 25://
			if(!MAIN_QR_PROCESSED)
			{
			    Set_tba_WheelLED(L_LED,SET);
				  Set_tba_WheelLED(R_LED,SET);
				  Mark=25;
			}	
			else
			{
				Set_tba_WheelLED(L_LED,RESET);
				Set_tba_WheelLED(R_LED,RESET);
				
				Mark=30;
			}	
			break;

		case 30://得到烽火台秘钥 并存储
			AGV_FH_1[3]=LICENSE_PROCESSED_DATA[4];
			AGV_FH_1[4]=LICENSE_PROCESSED_DATA[5];
			AGV_FH_1[5]=LICENSE_PROCESSED_DATA[6];
			AGV_FH_1[6]=(AGV_FH_1[2]+AGV_FH_1[3]+AGV_FH_1[4]+AGV_FH_1[5])%256;
			Send_ZigbeeData_To_Fifo(AGV_FH_1,8);
			
			Mark=31;
			break;
		
		case 31:
			AGV_FH_2[3]=LICENSE_PROCESSED_DATA[7];
			AGV_FH_2[4]=LICENSE_PROCESSED_DATA[8];
			AGV_FH_2[5]=LICENSE_PROCESSED_DATA[9];
			AGV_FH_2[6]=(AGV_FH_2[2]+AGV_FH_2[3]+AGV_FH_2[4]+AGV_FH_2[5])%256;
			Send_ZigbeeData_To_Fifo(AGV_FH_2,8);
			
			Mark=32;
			break;
		
		case 32://寻迹
			go_track(50);
			Mark=180;				
			break;
		
		case 180://直行
			go(50,300);
			Mark=35;				
			break;
		
		case 35://左转
			turn_left(95);
			Mark=36;				
			break;
		
		case 36://前进准备测路灯
			go_track_mp(30,400);
			Mark= 40;		
			break;
		
		case 40://开始测路灯
			AGV_CK_VAL=mode();			
			Mark=41;		
			break;
		
		case 41://开始测路灯
			back(30,300);	
			Mark=45;		
			break;
		
		case 45://右转
			turn_right(95);
			Mark=46;		
			break;
		
		case 46://寻迹到F2
			if(go_track(60)==1) 
			{
				if((CanHost_Mp-FULL_MP)<2000) //F2途中遇到卡
				{
					Mark=183;
//					SYN_TTS("遇到卡");
				} 				
			}
			else Mark=60;
			break;
			
		case 183:
			go_readcard();
//		  SYN_TTS("读卡完毕");
		  Mark=47;
			break;
			
		case 47://要求解密RFID
			RFID_DATA[2]=0x88;
			RFID_DATA[3]=0x09;
			RFID_DATA[4]=RFID_RX_buff[0];
			RFID_DATA[5]=RFID_RX_buff[1];
			RFID_DATA[6]=RFID_RX_buff[2];
			RFID_DATA[7]=RFID_RX_buff[3];
			RFID_DATA[8]=RFID_RX_buff[4];
			RFID_DATA[9]=RFID_RX_buff[5];
			RFID_DATA[10]=RFID_RX_buff[6];
			RFID_DATA[11]=RFID_RX_buff[7];
//			Send_WifiData_To_Fifo(RFID_DATA,12);
//			UartA72_TxClear();
//			UartA72_TxAddStr(RFID_DATA,12);
//			UartA72_TxStart();
//			delay_ms(20);
//		
//			RFID_DATA[2]=0x88;
//			RFID_DATA[3]=0x10;
			RFID_DATA[12]=RFID_RX_buff[8];
			RFID_DATA[13]=RFID_RX_buff[9];
			RFID_DATA[14]=RFID_RX_buff[10];
			RFID_DATA[15]=RFID_RX_buff[11];
			RFID_DATA[16]=RFID_RX_buff[12];
			RFID_DATA[17]=RFID_RX_buff[13];
			RFID_DATA[18]=RFID_RX_buff[14];
			RFID_DATA[19]=RFID_RX_buff[15];
			Send_WifiData_To_Fifo(RFID_DATA,20);
			UartA72_TxClear();
			UartA72_TxAddStr(RFID_DATA,20);
			UartA72_TxStart();
			
//			SYN_TTS("上传射频卡数据完毕");
			Mark=48;		
			break;
		
		case 48:
			if(!RFID_PROCESSED)
			{
			    Set_tba_WheelLED(L_LED,SET);
				  Set_tba_WheelLED(R_LED,SET);
				  Mark=48;
			}	
			else
			{
				Set_tba_WheelLED(L_LED,RESET);
				Set_tba_WheelLED(R_LED,RESET);
				AGV_FX=	RFID_PROCESSED_DATA[4];	
				if(AGV_FX==1) ; //SYN_TTS("车头方向为一");
				else if(AGV_FX==2) ; //SYN_TTS("车头方向为二");
				else if(AGV_FX==3) ; //SYN_TTS("车头方向为三");
				else if(AGV_FX==4)  ;//SYN_TTS("车头方向为四");
				else ;//SYN_TTS("解密错误");
				Mark=55;
			}
			break;
			
		case 55://继续寻迹到F2
			go_track(50);			
			Mark=60;
			break;
		
		case 60://要求识别交通灯
			Send_ZigbeeData_To_Fifo(Traffic_LED_Buf,12);//让交通灯进入识别模式
			delay_ms(200);
			Send_ZigbeeData_To_Fifo(Traffic_LED_Buf,12);//让交通灯进入识别模式
			Mark=189;
			break;
		
		case 189://要求识别交通灯
			Send_WifiData_To_Fifo(TRAFFIC_LIGHT_DATA,12);//发送请求识别交通灯
			delay_ms(200);
			Send_WifiData_To_Fifo(TRAFFIC_LIGHT_DATA,12);//发送请求识别交通灯
			Mark=61;
			break;
		
		//等待识别交通灯
		case 61:
			if(!TRAFFIC_LIGHT_PROCESSED)
			{
			    Set_tba_WheelLED(L_LED,SET);
				  Set_tba_WheelLED(R_LED,SET);
				  Mark=61;
			}	
			else
			{
				Set_tba_WheelLED(L_LED,RESET);
				Set_tba_WheelLED(R_LED,RESET);
				
				Mark=62;
			}					
			break;
		
		case 62://交通灯请求确认
			Traffic_verify_Buf[3]=TRAFFIC_LIGHT_PROCESSED_DATA[4];//将识别的颜色发送给交通灯
			Traffic_verify_Buf[6]=(Traffic_verify_Buf[2]+Traffic_verify_Buf[3]+Traffic_verify_Buf[4]+Traffic_verify_Buf[5])%256;
			Send_ZigbeeData_To_Fifo(Traffic_verify_Buf,8);
			Mark=63;
			break;
		
		case 63://前蹭
			go(60,300);
			Mark= 65;
			break;

		case 65://右转
			turn_right(95);
			Mark= 66;
			break;
			
		case 66://寻迹
			go_track(60);
			Mark= 67;
			break;
		
		case 67://前蹭
			go(60,300);
			Mark= 70;
			break;
		
		case 70://语音识别
			SYN_7318_One_test(1,2);
			Mark=71;
			break;

		case 71://右转
			turn_right(95);
			Mark=75;
			break;
			
		case 75://寻迹
			go_track(60);
			Mark=76;
			break;
		
		case 76://前进
			go(60,300);
			Mark= 80;
			break;
		
		case 80://寻迹码盘
			go_track_mp(60,1000); 
			Mark=81;
			break;
		
		
		//_____________________AGV开始____________________________
		case 81:
			Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8);//agv状态开始上传
			delay_ms(200);		
//			Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8);//agv状态开始上传
			if(AGV_FX==1) {Mark= 83;}
			else if(AGV_FX==2) {Mark= 90;}
			else if(AGV_FX==3) {Mark= 100;}
			else if(AGV_FX==4) {Mark= 95;}
			else Mark= 100;
			break;
		
		case 83://车头在1
			Send_InfoData_To_Fifo("83",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=83;
			}
			else {Mark=84;AGV_CMD_Sent=0;}
			break;
			
		case 84://车头在1
			Send_InfoData_To_Fifo("84",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=83;
			}
			else {Mark=100;AGV_CMD_Sent=0;}
			break;
			
		
		case 90://车头在2
			Send_InfoData_To_Fifo("90",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=90;
			}
			else {Mark=100;AGV_CMD_Sent=0;}
			break;
			
			case 95://车头在4
			Send_InfoData_To_Fifo("95",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv左转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=95;
			}
			else {Mark=100;AGV_CMD_Sent=0;}
			break;
		
		case 100://车头为3  开始前进
			Send_InfoData_To_Fifo("##100",5);		
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvx寻迹
			}			
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			AGV_CMD_Sent++;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=100;
			}
			else {Mark=105;AGV_CMD_Sent=0;}
			break;
			
		case 105://开始测距
			Follower_dis=Zigb_Rx_Buf[5]*256+Zigb_Rx_Buf[4];
			LED_Tx_Buff[4]=Follower_dis/100;
			LED_Tx_Buff[5]=(((Follower_dis%100)/10)<<4) | (((Follower_dis%100)%10)&0xff);
			delay_ms(500);

			Follower_dis=Zigb_Rx_Buf[5]*256+Zigb_Rx_Buf[4];
			LED_Tx_Buff[4]=Follower_dis/100;
			LED_Tx_Buff[5]=(((Follower_dis%100)/10)<<4) | (((Follower_dis%100)%10)&0xff);
			delay_ms(500);
			LED_Tx_Buff[6]=(LED_Tx_Buff[2]+LED_Tx_Buff[3]+LED_Tx_Buff[4]+LED_Tx_Buff[5])%256;
			Send_ZigbeeData_To_Fifo(LED_Tx_Buff,8);//发送标志物距离
			delay_ms(50);

			Mark=106;
			break;
		
		
		case 106://前蹭
			Send_InfoData_To_Fifo("##106",5);		
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}			
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			AGV_CMD_Sent++;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=106;
			}
			else {Mark=107;AGV_CMD_Sent=0;}			
			break;
			
		case 107://二维码求识别
			Send_InfoData_To_Fifo("##107",5);	
			Send_ZigbeeData_To_Fifo(AGV_START_READ_QR,8);		  
			Mark=110; 	
			
			break;
		case 110:                     //等待识别完成并发送要求解码数据
			Send_InfoData_To_Fifo("##110",5);	
      if(follower_Buff[0]!=0x55) 
			{
				Mark=110; 
			}		
      else
			{
					if(follower_Buff[3]==0x01)//识别成功
					{
						//要求解密
						//关闭从车上传数据：55028000000080BB						
						//开启主车上传主车数据： 55AA8000000080BB						
						//发送二维码数据(50字节)
						UartA72_TxClear();
						UartA72_TxAddStr(follower_Buff,50);
						UartA72_TxStart();
			      Send_WifiData_To_Fifo(follower_Buff,50);
						Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8); //还原AGV任务板上传
						Mark=111;
					}
					else if(follower_Buff[3]==0x02)//识别失败
					{
//					   SYN_TTS("从车二维码识别失败");
						Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8); //还原AGV任务板上传
						Mark=115; //直接做下一个动作
					}
			}		
			break;
			
			case 111://对解密数据进行处理
			Send_InfoData_To_Fifo("##111",5);	
//		  SYN_TTS("请求安卓对从车识别的二维码解密");		
		  Mark=112;
			break;
			
			
		
//		case 107:
//		  Send_WifiData_To_Fifo(QR_DATA,8);
//		  Mark=108;
//			break;
		
		case 112:
			Send_InfoData_To_Fifo("##112",5);	
			if(!QR_PROCESSED)
				{
						Set_tba_WheelLED(L_LED,SET);
						Set_tba_WheelLED(R_LED,SET);
						Mark=112;
				}	
				else
				{
					Set_tba_WheelLED(L_LED,RESET);
					Set_tba_WheelLED(R_LED,RESET);
					
					Mark=113;
				}
			break;
				
		case 113:		//解密主车车库层数
			Send_InfoData_To_Fifo("##113",5);	
			GARAGE_NO=QR_PROCESSED_DATA[4];
			Mark=115;
			break;		
		
		case 115:		
			Send_InfoData_To_Fifo("##115",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=115;
			}
			else {Mark=116;AGV_CMD_Sent=0;}
			break;
		
		case 116:		
			Send_InfoData_To_Fifo("##116",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=116;
			}
			else {Mark=117;AGV_CMD_Sent=0;}
			break;
		
		case 117:		
			Send_InfoData_To_Fifo("##117",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=117;
			}
			else {Mark=118;AGV_CMD_Sent=0;}
			break;
			
			case 118://发送烽火台命令
			Send_InfoData_To_Fifo("##118",5);	
			Send_ZigbeeData_To_Fifo(AGV_FH_3,8);
			Mark=119;
			break;
		
		case 119:			
			Send_InfoData_To_Fifo("##119",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=119;
			}
			else {Mark=120;AGV_CMD_Sent=0;}
			break;
			
		case 120:
			Send_InfoData_To_Fifo("##120",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=120;
			}
			else {Mark=121;AGV_CMD_Sent=0;}
			break;	
			
				case 121:
			if(AGV_CK_VAL==1) {Mark=122;}//1车库
			else if(AGV_CK_VAL==2) {Mark=125;}//2车库
			else if(AGV_CK_VAL==3) {Mark=130;}//3车库
			else if(AGV_CK_VAL==4) {Mark=140;}//4车库
			else Mark=122;
			break;
		
		case 122://1车库
			Send_InfoData_To_Fifo("##122",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjinMP_tx_Buff,8);
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=122;
			}
			else {Mark=150;AGV_CMD_Sent=0;}
			break;
		
		case 125://二车库
				Send_InfoData_To_Fifo("##125",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=125;
			}
			else {Mark=126;AGV_CMD_Sent=0;}
			break;
			
		
		case 126://二车库
			Send_InfoData_To_Fifo("##126",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=126;
			}
			else {Mark=127;AGV_CMD_Sent=0;}
			break;
			
		case 127://二车库
			Send_InfoData_To_Fifo("##127",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=127;
			}
			else {Mark=128;AGV_CMD_Sent=0;}
			break;	
		
		case 128://二车库
			Send_InfoData_To_Fifo("##128",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv左转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=128;
			}
			else {Mark=129;AGV_CMD_Sent=0;}
			break;
		
		case 129:	//二车库
			Send_InfoData_To_Fifo("##129",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjinMP_tx_Buff,8);
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=129;
			}
			else {Mark=150;AGV_CMD_Sent=0;}
			
			case 130://三车库
			Send_InfoData_To_Fifo("##130",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=130;
			}
			else {Mark=131;AGV_CMD_Sent=0;}
			break;
			
		
		case 131://三车库
			Send_InfoData_To_Fifo("##131",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=131;
			}
			else {Mark=132;AGV_CMD_Sent=0;}
			break;
			
		case 132://三车库
			Send_InfoData_To_Fifo("##132",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=132;
			}
			else {Mark=133;AGV_CMD_Sent=0;}
			break;	
			
		case 133://三车库
			Send_InfoData_To_Fifo("##133",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=133;
			}
			else {Mark=134;AGV_CMD_Sent=0;}
			break;
			
		case 134://三车库
			Send_InfoData_To_Fifo("##134",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=134;
			}
			else {Mark=135;AGV_CMD_Sent=0;}
			break;	
			
		case 135://三车库	
			Send_InfoData_To_Fifo("##135",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv左转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=135;
			}
			else {Mark=136;AGV_CMD_Sent=0;}
			break;
		
		case 136:	//三车库
			Send_InfoData_To_Fifo("##136",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjinMP_tx_Buff,8);//agv前进距离
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=136;
			}
			else {Mark=150;AGV_CMD_Sent=0;}
			break;
			
			case 140://四车库
			Send_InfoData_To_Fifo("##140",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv右转
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X02) 
			{
				Mark=140;
			}
			else {Mark=141;AGV_CMD_Sent=0;}
			break;
			
		
		case 141://四车库
			Send_InfoData_To_Fifo("##141",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=141;
			}
			else {Mark=142;AGV_CMD_Sent=0;}
			break;
			
		case 142://四车库
			Send_InfoData_To_Fifo("##142",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=142;
			}
			else {Mark=143;AGV_CMD_Sent=0;}
			break;	
			
		case 143://四车库
			Send_InfoData_To_Fifo("##143",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=143;
			}
			else {Mark=144;AGV_CMD_Sent=0;}
			break;
			
		case 144://四车库
			Send_InfoData_To_Fifo("##144",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agv前进
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=144;
			}
			else {Mark=145;AGV_CMD_Sent=0;}
			
			break;		
			
		case 145://四车库
			Send_InfoData_To_Fifo("##116",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agv寻迹
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X01) 
			{
				Mark=145;
			}
			else {Mark=146;AGV_CMD_Sent=0;}
			break;
			
		case 146://四车库
			Send_InfoData_To_Fifo("##120",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjinMP_tx_Buff,8);
			}
			AGV_CMD_Sent++;
			if(AGV_CMD_Sent>=10) AGV_CMD_Sent=0;
			delay_ms(20);
			if(Follower_Tab[2]!=0X03) 
			{
				Mark=146;
			}
			else {Mark=150;AGV_CMD_Sent=0;}
			
			break;
			
		//_____________________AGV完成____________________________	
			
			case 150://寻迹
			go_track(60);
			Mark=151;				
			break;
		
		case 151://前进
			go(60,300);
			Mark=152;				
			break;
			
		
		case 152://左转
			turn_left(95);
			Mark=155;				
			break;
			
		case 155://寻迹
			go_track(60);
			Mark=156;				
			break;
		
		case 156://前进
			go(60,300);
			Mark=157;				
			break;
		
		case 157://左转
			turn_left(95);
			Mark=158;				
			break;	
			
		case 158://寻迹
			go_track(60);
//		  SYN_TTS("遇到特殊地形");
			Mark=159;			
			break;	
		
		case 159://冲到靠近F2前
			go(60,1200);
//		   SYN_TTS("冲过去了");
			Mark=160;			
			break;
		
		case 160://寻迹
			go_track(60);
			Mark=161;//16			
			break;
			
		case 161://前进
			go(60,300);
			Mark=162;				
			break;	
			
		case 162://左转
			turn_left(95);
			Mark=163;				
			break;	
			
		case 163:
			go_track_mp(30,1000);             //循迹使车身正
			Mark=164;
			break;
			
		case 164:                          //后退进车库
			back(30,2100);
			Mark=165;
			break;	
			
		case 165:
			garage_Tx_Buff[3]=GARAGE_NO;//车库到达第一层
			garage_Tx_Buff[6]=(garage_Tx_Buff[2]+garage_Tx_Buff[3]+garage_Tx_Buff[4]+garage_Tx_Buff[5])%256;
			Send_ZigbeeData_To_Fifo(garage_Tx_Buff,8);
			Mark=166;
			break;
		
		case 166:                          //后退进车库
			Send_ZigbeeData_To_Fifo(WUXIAN_tx_Buff,8);//无线充电
			delay_ms(200);
			Send_ZigbeeData_To_Fifo(jishioff_tx_Buff,8);//计时结束
		  Full_Enable=0;
			Mark=0;
			break;
	
		default:	
			break;		
			
		
	}
}


/**
函数功能：保存ZigBee数据
参    数: 无
返 回 值：无
*/
void Can_ZigBeeRx_Save(uint8_t res)
{
/*	if(Zigbee_Rx_flag == 0)
	{
		Zigb_Rx_Buf[Zigbee_Rx_num]=res;
		Zigbee_Rx_num++;
		if(Zigbee_Rx_num > ZIGB_RX_MAX )	
		{
			Zigbee_Rx_num = 0;
			Zigbee_Rx_flag = 1;
		} 
	} */

	if(Zigbee_Rx_flag == 0)
	{
		canu_zibe_rxtime = gt_get()+10;
		Zigbee_Rx_num =0;
		Zigb_Rx_Buf[Zigbee_Rx_num]=res;
		Zigbee_Rx_flag = 1;
	}
	else if(Zigbee_Rx_num < ZIGB_RX_MAX )	
	{
		Zigb_Rx_Buf[++Zigbee_Rx_num]=res;	 
	}
		
}


/**
函数功能：ZigBee数据监测
参    数：无
返 回 值：无
*/
void Can_ZigBeeRx_Check(void)
{
	if(Zigbee_Rx_flag)
	{
		if(gt_get_sub(canu_zibe_rxtime) == 0)
		{
			if(Zigb_Rx_Buf[1] == 0x03) 			// 道闸 
			{
			   if(Zigb_Rx_Buf[2]==0x01)
			    {				
						Stop_Flag=Zigb_Rx_Buf[4];	  			 																		
			    }		
			} 
			else if(Zigb_Rx_Buf[1]==0x0c) 		// ETC
			{
				if(Zigb_Rx_Buf[2]==0x01)
			    {
					if(Zigb_Rx_Buf[3]==0x01)
					{
						Stop_Flag=Zigb_Rx_Buf[4] ;	  			 											
					}							
			    }		
			}
			else if((Zigb_Rx_Buf[1]==0x0E)&&(Zigb_Rx_Buf[2]==0x01))	//交通灯标志物					
			{
				Stop_Flag = Zigb_Rx_Buf[4] ;	  						// 0x07进入识别模式 0x08 未进入识别模式
			} 
			else if((Zigb_Rx_Buf[1]==0x0D)&&(Zigb_Rx_Buf[2]==0x03)) //立体车库标志物
			{
				if(Zigb_Rx_Buf[3] == 0x01)							//获取立体车库当前层数
				{
					switch(Zigb_Rx_Buf[4])							
					{
						case 1:
							Stop_Flag = 0x09;						//第一层
							break;
						case 2:
							Stop_Flag = 0x0A;						//第二层
							break;
						case 3:
							Stop_Flag = 0x0B;						//第三层
							break;
						case 4:
							Stop_Flag = 0x0C;						//第四层
							break;
					}
				} 
				else if(Zigb_Rx_Buf[3] == 0x02) 					//获取立体车库当前红外状态
				{
					if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x11;							//前侧红外触发、后侧红外触发
					} 
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x22;							//前侧红外未触发、后侧红外未触发
					}
					else if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x12;							//前侧红外触发、后侧红外未触发
					}
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x21;							//前侧红外未触发、后侧红外触发
					}
				}
			}
			else if((Zigb_Rx_Buf[0] == 0x55)&&(Zigb_Rx_Buf[1] == 0x02))		//返回从车数据
			{		
					
					if(Zigb_Rx_Buf[2]==0x92)
					{
						memcpy(follower_Buff,Zigb_Rx_Buf,50);
						follower_Buff[1]=0xaa;
						Zigb_Rx_Buf[2]=0x00;
					}		
					else	
					{
						memcpy(Follower_Tab,Zigb_Rx_Buf,50);
					}						
					AGV_data_Falg = SET;
				
			}
			else if(Zigb_Rx_Buf[1]==0x06)	 //语音播报返回
			{
				if(Zigb_Rx_Buf[2]==0x01)
				{
						Stop_Flag=Zigb_Rx_Buf[3] ;	  // 语音芯片状态返回
				}			
			}
			Zigbee_Rx_flag = 0;
		}	
	}
}


/** 暂未使用
函数功能：设置循迹上传更新时间
参    数：无
返 回 值：无
*/
void Canuser_upTrackTime(void)
{
	static uint8_t run_mode = 0;
	
	if(gt_get() < 1000)
		run_mode = 0;
	else if(run_mode == 0)
	{
		run_mode = 1;
		Host_Set_UpTrack(50);
	}
}

#define UART_or_CAN    0   //  0---UART  1---CAN

/**
函数功能：CAN查询、发送接收检测
参    数：无
返 回 值：无
*/
void Canuser_main(void)
{
	CanP_Host_Main();
	CanP_CanTx_Check();				//CAN总线发送数据监测
}



//end file

