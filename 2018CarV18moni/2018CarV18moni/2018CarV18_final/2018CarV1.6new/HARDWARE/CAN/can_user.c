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

#define delay_Time 1500   //5ms�ı���
u8 LED_Tx_Buff[8]={0x55,0x04,0X04,0x00,0x00,0x00,0x00,0xBB};//LED��ʾ�ڶ��ž��� ����λ����λΪ����
u8 jishi_tx_Buff[8]={0x55,0x04,0X03,0X02,0X00,0X00,0X05,0XBB};//��ʱ����
u8 jishiopen_tx_Buff[8]={0x55,0x04,0X03,0X01,0X00,0X00,0X04,0XBB};//��ʱ��
u8 jishioff_tx_Buff[8]={0x55,0x04,0X03,0X00,0X00,0X00,0X03,0XBB};//��ʱ��

u8 garage_Tx_Buff[8]={0x55,0x0D,0X01,0x00,0x00,0x00,0x00,0xBB};//�������ָ��

u8 yuyin_tx_Buff[13]={0xFD,0x00,0x0A,0x01,0x01,0xB4,0xF2,0xBF,0xAA,0xD3,0xEF,0xD2,0xF4};//����ָ������

u8 daoza_tx_Buff[8]={0x55,0x03,0x01,0x01,0x00,0x00,0x02,0xBB};//��բ��

u8 agvflag_tx_Buff[8]={0x55,0x02,0x80,0x01,0x00,0x00,0x81,0xbb};//�ϴ��ӳ�״̬
u8 agvflagoff_tx_Buff[8]={0x55,0x02,0x80,0x00,0x00,0x00,0x80,0xbb};//�ر��ϴ��ӳ�״̬

u8 agv_tx_Buff[8]={0X55,0X02,0X06,0X32,0X00,0X00,0X38,0XBB};  //agvѭ��
u8 agvqianjin_tx_Buff[8]={0X55,0X02,0X02,0X60,0X90,0X01,0Xf3,0XBB};  //agvǰ��   ����ֵ400
u8 agvqianjin1500_tx_Buff[8]={0X55,0X02,0X02,0X60,0Xdc,0X05,0X43,0XBB};  //agvǰ��1500	60��96  5dc��1500
u8 agvright_tx_Buff[8]={0X55,0X02,0X05,0X60,0X00,0X00,0X65,0XBB};	//agv��ת���� 96�ٶ�  ������ʮ������
u8 agvleft_tx_Buff[8]={0X55,0X02,0X04,0X60,0X00,0X00,0X64,0XBB};	//agv��ת���� 96�ٶ�	
u8 agvqianjinMP_tx_Buff[]={0X55,0X02,0X02,0X60,0X58,0X02,0Xbc,0XBB};  //agvǰ��1500	60��96  258��600  ������

u8 WUXIAN_tx_Buff[8]={0X55,0X0A,0X01,0X01,0X00,0X00,0X02,0XBB};//���߳��
u8 ZHUflag_tx_Buff[8]={0x55,0xAA,0x80,0x01,0x00,0x00,0x81,0xbb};//����״̬�ϴ�
u8 AGV_START_READ_QR[]={0X55,0X02,0X92,0X01,0X00,0X00,0X93,0XBB};//��ʼʶ���ά��
u8 IR1_TX_BUFF[]={0x03,0x05,0x14,0x45,0xde,0x92};    //����̨
u8 ludeng_TX_BUFF[]={0X55,0XAA,0X63,0X00,0X00,0X00,0X63,0XBB};  //·��
u8 RFID_DATA[30]={0X55,0XAA}; //RFID��ԭʼ���� 

u8 Traffic_LED_Buf[8]={0X55, 0X0E, 0X01, 0X00, 0X00, 0X00, 0X01, 0XBB}; //��ͨ�ƽ���ʶ��ģʽ
u8 Traffic_verify_Buf[8]={0X55, 0X0E, 0X02, 0X01, 0X00, 0X00, 0X03, 0XBB}; //��ͨ��ʶ��������ȷ��  ���̻�  Ĭ�Ϻ�

u8 TFT_buf[]={0X55 ,0X0b, 0X10, 0X00, 0X01, 0X00, 0X00, 0XBB};//TFT��ʾĳ��ͼƬ




uint8_t Wifi_Rx_Buf[ WIFI_MAX_NUM ];
uint8_t Zigb_Rx_Buf[ ZIGB_RX_MAX ];
uint8_t Wifi_Rx_num ;
uint8_t Wifi_Rx_flag ;        //wifi������ɱ�־λ
uint8_t Zigbee_Rx_num ;
uint8_t Zigbee_Rx_flag ;      //zigbee������ɱ�־λ

uint8_t Host_AGV_Return_Flag = RESET;
uint8_t AGV_data_Falg = RESET;

uint8_t AGV_CMD_Sent=0;

uint32_t canu_wifi_rxtime = 0;
uint32_t canu_zibe_rxtime = 0;

u8 Wifi_Rx_Buf_Temp[17];//wifiתASCII������
u8 follower_Buff[50];//�ӳ���ά������
u8 Full_Enable=0;//ȫ�Զ�ʹ��
u8 Mark;//��������
u8 FIND_OK=0;//Ѱ��RFIF����־
u8 RFID_READED=0;//
u16 FULL_MP=0;//ȫ�Զ���ʻ����ֵ
u16 Follower_dis=0;//�ӳ��������������
extern uint8_t  rfid_return_premit;
extern uint8_t  delay_Left_flag;
extern uint8_t  delay_Right_flag;
extern uint32_t delay_Turn;
extern uint8_t	QR_flag;      //Android�����������شӳ�ʶ��Ķ�ά���־
extern u8 RFID_RX_buff[17];
extern uint8_t Track_buf[2];

//����������ݸ�ʽ��55 02 92 01 1E 3C 33 2C 34 2C 35 2C 37 2C 38 2C 39 2C 31 31 3E 2F 3C 31 2C 32 2C 33 2C 34 2C 35 2C 36 3E BB 00 00 00 00 00 00 00 00 00 00 00 00 00
uint8_t QR_PROCESSED_DATA[20];       //�ӳ���ά����ܺ������:             66 AA 93 ** ** ** ** ** ** ** ** BB     (������20�ֽ�)

//����ʶ�����������ά�����ݸ�ʽ��0x    55 AA A0 01 00 00 00 BB
uint8_t MAIN_QR_DATA[]={  0x55 ,0xAA, 0xA0, 0x01, 0x00,0x00, 0xA1, 0xBB};                                  
uint8_t MAIN_QR_PROCESSED_DATA[20];  //������ά����ܺ������:             66 AA A0 len ** ** ** ** ** ** ** ** BB (������20�ֽ�)

//RFID����������ݸ�ʽ��
//55 AA 88 09 48 45 4C 4C 4F 5F 4D 52 
//55 AA 88 10 43 35 32 32 2E 2E 2E 2E 
uint8_t RFID_PROCESSED_DATA[20];   //RFID���ܺ������               66 AA 88 len ** ** ** ** ** ** ** ** BB(������20�ֽ�)
//��������androidʶ���Ƹ�ʽ��  55 AA 94 01 00 00 00 00 00 00 00 00 
uint8_t LICENSE_DATA[]={0x55 ,0xAA ,0x94 ,0x01 ,0x00 ,0x00 ,0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00};             
uint8_t LICENSE_PROCESSED_DATA[20];//����ʶ����������           66 AA 94 len ** ** ** ** ** ** ** ** BB(������20�ֽ�)
 

//��������androidʶ��ͼ�θ�ʽ��  55 AA 96 01 00 00 00 00 00 00 00 00 
uint8_t GRAPHIC_DATA[]={0x55 ,0xAA ,0x96 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 };                                  
uint8_t GRAPHIC_PROCESSED_DATA[20];//ͼ��ʶ����������           66 AA 96 len (A)(0*) (B) (0*) (C) (0*) (D) (0*) (E) (0*) BB (������20�ֽ�)

 //��������androidʶ��ͨ�Ƹ�ʽ��55 AA 98 01 00 00 99 BB
uint8_t TRAFFIC_LIGHT_DATA[]= {0x55, 0xAA ,0x98 ,0x01 ,0x00 ,0x00 ,0x99 ,0xBB};  
uint8_t TRAFFIC_LIGHT_PROCESSED_DATA[20];//��ͨ��ʶ����������   66 AA 98 len 1/2/3(��/��/��) ** ** ** ** ** ** ** BB  (������20�ֽ�)

u8  RFID_PROCESSED=0;    //RFID������ɱ�־
u8  QR_PROCESSED=0;      //�ӳ���ά�������ɱ�־
u8  MAIN_QR_PROCESSED=0; //������ά��ʶ�������ɱ�־
u8  LICENSE_PROCESSED=0; //����ʶ�������ɱ�־
u8  GRAPHIC_PROCESSED=0; //ͼ��ʶ�������ɱ�־
u8  TRAFFIC_LIGHT_PROCESSED=0; //��ͨ��ʶ�������ɱ�־
u8  GARAGE_NO;//�ӽ���������ȡ�ó������
u8  AGV_FX;//����������ȡ�ôӳ�����
u8	AGV_CK_VAL;//AGVС������

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

void Normal_data(void)	  // ��������8�ֽڿ���ָ��
{
	u8 sum=0;

	if(Wifi_Rx_Buf[7]==0xbb)	 // �жϰ�β
	 {									  
		//��ָ������λ��ָ�������У��
		//ע�⣺��������ʱӦ�öԺ���256ȡ�ࡣ
		 sum=(Wifi_Rx_Buf[2]+Wifi_Rx_Buf[3]+Wifi_Rx_Buf[4]+Wifi_Rx_Buf[5])%256;
		if(sum == Wifi_Rx_Buf[6])
		{
			   Rx_Flag =1;
		}
		else Rx_Flag =0;
	}
}

void Abnormal_data(void)	  //�����쳣����
{
	u8 i,j;
	u8 sum=0;
	
	if(Wifi_Rx_num <8)			// �쳣�����ֽ���С��8�ֽڲ�������
	{
	   Rx_Flag =0;
	}
	else {
		for(i=0;i<=(Wifi_Rx_num -7);i++)  
		{
			if(Wifi_Rx_Buf[i]==0x55)	   // Ѱ�Ұ�ͷ
			{
			   if(Wifi_Rx_Buf[i+7]==0xbb)	// �жϰ�β
			   {
			   	    sum=(Wifi_Rx_Buf[i+2]+Wifi_Rx_Buf[i+3]+Wifi_Rx_Buf[i+4]+Wifi_Rx_Buf[i+5])%256;

		           if(sum==Wifi_Rx_Buf[i+6])	 // �ж����
	              {
			         for(j=0;j<8;j++)
					 {
					 	 Wifi_Rx_Buf[j]=Wifi_Rx_Buf[j+i];	 // ���ݰ���
					 }
					    Rx_Flag =1;
		          }
	  	         else Rx_Flag =0;
		       }
			}
	    }

	}
}

uint8_t Infrared_Tab[6];			//�������ݴ������

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
			else if(Wifi_Rx_Buf[0]==0x66) //Android���صĽ�����Ϣ��ʽ
			{	
				if(Wifi_Rx_Buf[1]==0xAA)
				{					
					switch(Wifi_Rx_Buf[2])
					{
						case 0x88:  //ANDROID����RFID����������
							strncpy(RFID_PROCESSED_DATA,Wifi_Rx_Buf,20);       RFID_PROCESSED=1;  //��ʾ��RFID�������
							break;
						case 0x93:  //ANDROID���شӳ���ά���������  
							strncpy(QR_PROCESSED_DATA,Wifi_Rx_Buf,20);         QR_PROCESSED=1;
							break;
						case 0x94:  //ANDROID�����������ƽ�������
							strncpy(LICENSE_PROCESSED_DATA,Wifi_Rx_Buf,20);     LICENSE_PROCESSED=1;
							break;
						case 0x96:  //ANDROID����ͼ�ν�������  
							strncpy(GRAPHIC_PROCESSED_DATA,Wifi_Rx_Buf,20);     GRAPHIC_PROCESSED=1;
							break; 
						case 0x98:  //ANDROID���ؽ�ͨ�ƽ�������  
							strncpy(TRAFFIC_LIGHT_PROCESSED_DATA,Wifi_Rx_Buf,20);TRAFFIC_LIGHT_PROCESSED=1;
						break;  
						case 0xA0:  //ANDROID����������ά���������  
							strncpy(MAIN_QR_PROCESSED_DATA,Wifi_Rx_Buf,20);      MAIN_QR_PROCESSED=1;
						break;
						default: break;
					}
			 }
			}
			else if(Wifi_Rx_Buf[0]==0x55)  //Android�·��Ŀ���ָ��
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
			case 0x01:              	//ֹͣ
				Send_UpMotor(0 ,0);
				Roadway_Flag_clean();	//�����־λ״̬	
				break;
			case 0x02:              //ǰ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				Control(Car_Spend ,Car_Spend);
				break;
			case 0x03:              //����
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 1; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Control(-Car_Spend ,-Car_Spend);					
				break;
			case 0x04:              //��ת
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];				
				Control(-Car_Spend ,Car_Spend);
				break;
			case 0x05:              //��ת
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];
				Control(Car_Spend,-Car_Spend);
				break;
			case 0x06:              //ѭ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 1;
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				break;
			case 0x07:				//��������
				
				break;
			case 0x08:				//��ת��--�Ƕ�											
				Roadway_nav_syn();	//�Ƕ�ͬ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];				
				Send_UpMotor(-Car_Spend ,Car_Spend);					
				break;
			case 0x09:				//��ת��--�Ƕ�			
				Roadway_nav_syn();	//�Ƕ�ͬ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Send_UpMotor(Car_Spend,-Car_Spend);
				break;
			case 0x10:										//����ǰ��λ����
				Infrared_Tab[0]=Wifi_Rx_Buf[3];
				Infrared_Tab[1]=Wifi_Rx_Buf[4];
				Infrared_Tab[2]=Wifi_Rx_Buf[5];
				break;
			case 0x11:										//�������λ����
				Infrared_Tab[3]=Wifi_Rx_Buf[3];//���ݵ���λ
				Infrared_Tab[4]=Wifi_Rx_Buf[4];//��λУ����
				Infrared_Tab[5]=Wifi_Rx_Buf[5];//��λУ����
				break;
			case 0x12:										//֪ͨС����Ƭ�����ͺ�����
				Infrared_Send(Infrared_Tab,6);
				
				break;									
			case 0x20:	//ת��ƿ���
				Set_tba_WheelLED(L_LED,Wifi_Rx_Buf[3]);
				Set_tba_WheelLED(R_LED,Wifi_Rx_Buf[4]);
				break;					
			case 0x30:
				Set_tba_Beep(Wifi_Rx_Buf[3]);				//������
				break;
			case 0x40:										//��δʹ��
				
				break;
				case 0x50:  				//���ⷢ�������Ƭ�Ϸ� 
				Infrared_Send(H_S,4);
				break;
			case 0x51:    					//���ⷢ�������Ƭ�·� 
				Infrared_Send(H_X,4);
				break;
			case 0x61:    					//���ⷢ����ƹ�Դǿ�ȵ�λ��1 
				Infrared_Send(H_1,4);
				Send_InfoData_To_Fifo("1\n",3);
				break;
			case 0x62:   	 				//���ⷢ����ƹ�Դǿ�ȵ�λ��2 
				Infrared_Send(H_2,4);
				Send_InfoData_To_Fifo("2\n",3);
				break;
			case 0x63:    					//���ⷢ����ƹ�Դǿ�ȵ�λ��3 
				Infrared_Send(H_3,4);
				Send_InfoData_To_Fifo("3\n",3);
				break;
		   case 0x80:						//�˶���־�����ݷ�������λ
			    Host_AGV_Return_Flag = Wifi_Rx_Buf[3];   //SET ���� / RESET ��ֹ
				break;
//			 case 0x88: rfid_return_premit=1; //������ȡRFID����
//        break;
			 case 0x14: delay_Left_flag=1; delay_Turn = gt_get() +delay_Time;//��ȡ��ǰʱ��//����������ʱ��ת����
        break;
			 case 0x15: delay_Right_flag=1; delay_Turn = gt_get() +delay_Time;//��ȡ��ǰʱ��//����������ʱ��ת����
        break;
			 case 0x92:            //ANDROID�����������شӳ�ʶ��Ķ�ά����������
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
			 
			 case 78://�ӳ�������ʾ������ʾ����
				  Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW1,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW2,8);
					delay_ms(30);
			 Send_ZigbeeData_To_Fifo(AGV_JULI_SHOW3,8);
				 break;
			 
			 case 0xFF: Full_Enable=1;Mark=4;SYN_TTS("ȫ�Զ�����ʼ");//��ȡ��ǰʱ��//����������ʱ��ת���� 
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
		//stop_flag:  01ʮ��·�� 02ת�� 03ǰ������ 04ȫ�� 0F ѭ������ﵽ�趨ֵ
		
			case 4:
			Send_ZigbeeData_To_Fifo(jishi_tx_Buff,8);//��ʱ����
			delay_ms(20);
			Mark=5;
			break;
		
		case 5:
			Send_ZigbeeData_To_Fifo(SMG_JSK,8);//��ʱ��ʼ
			delay_ms(20);
		  FULL_MP=CanHost_Mp;
			Mark=6;
		  break;
		
		case 6://Ѱ��		
			go_track(50);
			Mark=7;
			break;
		
		case 7://ǰ��
			go(50,300);
			Mark=10;
			break;
		
		case 10://Ѱ�����뵽����ʶ���ά�봦
		  if(go_track_mp(30,1050)==1)
			{
				if((CanHost_Mp-FULL_MP)<1200) //����D2ǰ�������а�·����RFID��
					Mark=12; //��������ʶ���ά��Ҫ��				
//				SYN_TTS("������");
			}
			else Mark=20;	//;��û������
			break;			
		
		case 12:			
      go_readcard();
//		  SYN_TTS("�������");
		  Mark=13;
			break;	
		
		case 13://����RFID����
			
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
			
//			SYN_TTS("�ϴ���Ƶ���������");
			Mark=14;
			break;		
			
		case 14 : //�ȴ�RFID�������			
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
				if(AGV_FX==1) ; //SYN_TTS("��ͷ����Ϊһ");
				else if(AGV_FX==2);  //SYN_TTS("��ͷ����Ϊ��");
				else if(AGV_FX==3)  ;//SYN_TTS("��ͷ����Ϊ��");
				else if(AGV_FX==4) ; //SYN_TTS("��ͷ����Ϊ��");
				else ;//SYN_TTS("���ܴ���");
				Mark=20;
			}			
			break;
			
		case 20://�������������ά��
			Send_WifiData_To_Fifo(MAIN_QR_DATA,12);
//			SYN_TTS("�������������ά��");
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

		case 30://�õ����̨��Կ ���洢
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
		
		case 32://Ѱ��
			go_track(50);
			Mark=180;				
			break;
		
		case 180://ֱ��
			go(50,300);
			Mark=35;				
			break;
		
		case 35://��ת
			turn_left(95);
			Mark=36;				
			break;
		
		case 36://ǰ��׼����·��
			go_track_mp(30,400);
			Mark= 40;		
			break;
		
		case 40://��ʼ��·��
			AGV_CK_VAL=mode();			
			Mark=41;		
			break;
		
		case 41://��ʼ��·��
			back(30,300);	
			Mark=45;		
			break;
		
		case 45://��ת
			turn_right(95);
			Mark=46;		
			break;
		
		case 46://Ѱ����F2
			if(go_track(60)==1) 
			{
				if((CanHost_Mp-FULL_MP)<2000) //F2;��������
				{
					Mark=183;
//					SYN_TTS("������");
				} 				
			}
			else Mark=60;
			break;
			
		case 183:
			go_readcard();
//		  SYN_TTS("�������");
		  Mark=47;
			break;
			
		case 47://Ҫ�����RFID
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
			
//			SYN_TTS("�ϴ���Ƶ���������");
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
				if(AGV_FX==1) ; //SYN_TTS("��ͷ����Ϊһ");
				else if(AGV_FX==2) ; //SYN_TTS("��ͷ����Ϊ��");
				else if(AGV_FX==3) ; //SYN_TTS("��ͷ����Ϊ��");
				else if(AGV_FX==4)  ;//SYN_TTS("��ͷ����Ϊ��");
				else ;//SYN_TTS("���ܴ���");
				Mark=55;
			}
			break;
			
		case 55://����Ѱ����F2
			go_track(50);			
			Mark=60;
			break;
		
		case 60://Ҫ��ʶ��ͨ��
			Send_ZigbeeData_To_Fifo(Traffic_LED_Buf,12);//�ý�ͨ�ƽ���ʶ��ģʽ
			delay_ms(200);
			Send_ZigbeeData_To_Fifo(Traffic_LED_Buf,12);//�ý�ͨ�ƽ���ʶ��ģʽ
			Mark=189;
			break;
		
		case 189://Ҫ��ʶ��ͨ��
			Send_WifiData_To_Fifo(TRAFFIC_LIGHT_DATA,12);//��������ʶ��ͨ��
			delay_ms(200);
			Send_WifiData_To_Fifo(TRAFFIC_LIGHT_DATA,12);//��������ʶ��ͨ��
			Mark=61;
			break;
		
		//�ȴ�ʶ��ͨ��
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
		
		case 62://��ͨ������ȷ��
			Traffic_verify_Buf[3]=TRAFFIC_LIGHT_PROCESSED_DATA[4];//��ʶ�����ɫ���͸���ͨ��
			Traffic_verify_Buf[6]=(Traffic_verify_Buf[2]+Traffic_verify_Buf[3]+Traffic_verify_Buf[4]+Traffic_verify_Buf[5])%256;
			Send_ZigbeeData_To_Fifo(Traffic_verify_Buf,8);
			Mark=63;
			break;
		
		case 63://ǰ��
			go(60,300);
			Mark= 65;
			break;

		case 65://��ת
			turn_right(95);
			Mark= 66;
			break;
			
		case 66://Ѱ��
			go_track(60);
			Mark= 67;
			break;
		
		case 67://ǰ��
			go(60,300);
			Mark= 70;
			break;
		
		case 70://����ʶ��
			SYN_7318_One_test(1,2);
			Mark=71;
			break;

		case 71://��ת
			turn_right(95);
			Mark=75;
			break;
			
		case 75://Ѱ��
			go_track(60);
			Mark=76;
			break;
		
		case 76://ǰ��
			go(60,300);
			Mark= 80;
			break;
		
		case 80://Ѱ������
			go_track_mp(60,1000); 
			Mark=81;
			break;
		
		
		//_____________________AGV��ʼ____________________________
		case 81:
			Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8);//agv״̬��ʼ�ϴ�
			delay_ms(200);		
//			Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8);//agv״̬��ʼ�ϴ�
			if(AGV_FX==1) {Mark= 83;}
			else if(AGV_FX==2) {Mark= 90;}
			else if(AGV_FX==3) {Mark= 100;}
			else if(AGV_FX==4) {Mark= 95;}
			else Mark= 100;
			break;
		
		case 83://��ͷ��1
			Send_InfoData_To_Fifo("83",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
		case 84://��ͷ��1
			Send_InfoData_To_Fifo("84",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
		
		case 90://��ͷ��2
			Send_InfoData_To_Fifo("90",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
			case 95://��ͷ��4
			Send_InfoData_To_Fifo("95",4);
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv��ת
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
		
		case 100://��ͷΪ3  ��ʼǰ��
			Send_InfoData_To_Fifo("##100",5);		
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvxѰ��
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
			
		case 105://��ʼ���
			Follower_dis=Zigb_Rx_Buf[5]*256+Zigb_Rx_Buf[4];
			LED_Tx_Buff[4]=Follower_dis/100;
			LED_Tx_Buff[5]=(((Follower_dis%100)/10)<<4) | (((Follower_dis%100)%10)&0xff);
			delay_ms(500);

			Follower_dis=Zigb_Rx_Buf[5]*256+Zigb_Rx_Buf[4];
			LED_Tx_Buff[4]=Follower_dis/100;
			LED_Tx_Buff[5]=(((Follower_dis%100)/10)<<4) | (((Follower_dis%100)%10)&0xff);
			delay_ms(500);
			LED_Tx_Buff[6]=(LED_Tx_Buff[2]+LED_Tx_Buff[3]+LED_Tx_Buff[4]+LED_Tx_Buff[5])%256;
			Send_ZigbeeData_To_Fifo(LED_Tx_Buff,8);//���ͱ�־�����
			delay_ms(50);

			Mark=106;
			break;
		
		
		case 106://ǰ��
			Send_InfoData_To_Fifo("##106",5);		
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
		case 107://��ά����ʶ��
			Send_InfoData_To_Fifo("##107",5);	
			Send_ZigbeeData_To_Fifo(AGV_START_READ_QR,8);		  
			Mark=110; 	
			
			break;
		case 110:                     //�ȴ�ʶ����ɲ�����Ҫ���������
			Send_InfoData_To_Fifo("##110",5);	
      if(follower_Buff[0]!=0x55) 
			{
				Mark=110; 
			}		
      else
			{
					if(follower_Buff[3]==0x01)//ʶ��ɹ�
					{
						//Ҫ�����
						//�رմӳ��ϴ����ݣ�55028000000080BB						
						//���������ϴ��������ݣ� 55AA8000000080BB						
						//���Ͷ�ά������(50�ֽ�)
						UartA72_TxClear();
						UartA72_TxAddStr(follower_Buff,50);
						UartA72_TxStart();
			      Send_WifiData_To_Fifo(follower_Buff,50);
						Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8); //��ԭAGV������ϴ�
						Mark=111;
					}
					else if(follower_Buff[3]==0x02)//ʶ��ʧ��
					{
//					   SYN_TTS("�ӳ���ά��ʶ��ʧ��");
						Send_ZigbeeData_To_Fifo(agvflag_tx_Buff,8); //��ԭAGV������ϴ�
						Mark=115; //ֱ������һ������
					}
			}		
			break;
			
			case 111://�Խ������ݽ��д���
			Send_InfoData_To_Fifo("##111",5);	
//		  SYN_TTS("����׿�Դӳ�ʶ��Ķ�ά�����");		
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
				
		case 113:		//���������������
			Send_InfoData_To_Fifo("##113",5);	
			GARAGE_NO=QR_PROCESSED_DATA[4];
			Mark=115;
			break;		
		
		case 115:		
			Send_InfoData_To_Fifo("##115",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
			case 118://���ͷ��̨����
			Send_InfoData_To_Fifo("##118",5);	
			Send_ZigbeeData_To_Fifo(AGV_FH_3,8);
			Mark=119;
			break;
		
		case 119:			
			Send_InfoData_To_Fifo("##119",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			if(AGV_CK_VAL==1) {Mark=122;}//1����
			else if(AGV_CK_VAL==2) {Mark=125;}//2����
			else if(AGV_CK_VAL==3) {Mark=130;}//3����
			else if(AGV_CK_VAL==4) {Mark=140;}//4����
			else Mark=122;
			break;
		
		case 122://1����
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
		
		case 125://������
				Send_InfoData_To_Fifo("##125",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
		
		case 126://������
			Send_InfoData_To_Fifo("##126",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 127://������
			Send_InfoData_To_Fifo("##127",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
		
		case 128://������
			Send_InfoData_To_Fifo("##128",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv��ת
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
		
		case 129:	//������
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
			
			case 130://������
			Send_InfoData_To_Fifo("##130",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
		
		case 131://������
			Send_InfoData_To_Fifo("##131",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 132://������
			Send_InfoData_To_Fifo("##132",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
		case 133://������
			Send_InfoData_To_Fifo("##133",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 134://������
			Send_InfoData_To_Fifo("##134",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
		case 135://������	
			Send_InfoData_To_Fifo("##135",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvleft_tx_Buff,8);//agv��ת
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
		
		case 136:	//������
			Send_InfoData_To_Fifo("##136",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjinMP_tx_Buff,8);//agvǰ������
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
			
			case 140://�ĳ���
			Send_InfoData_To_Fifo("##140",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvright_tx_Buff,8);//agv��ת
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
			
		
		case 141://�ĳ���
			Send_InfoData_To_Fifo("##141",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 142://�ĳ���
			Send_InfoData_To_Fifo("##142",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
		case 143://�ĳ���
			Send_InfoData_To_Fifo("##143",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 144://�ĳ���
			Send_InfoData_To_Fifo("##144",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agvqianjin_tx_Buff,8);//agvǰ��
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
			
		case 145://�ĳ���
			Send_InfoData_To_Fifo("##116",5);	
			if(!AGV_CMD_Sent)
			{
				Send_ZigbeeData_To_Fifo(agv_tx_Buff,8);//agvѰ��
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
			
		case 146://�ĳ���
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
			
		//_____________________AGV���____________________________	
			
			case 150://Ѱ��
			go_track(60);
			Mark=151;				
			break;
		
		case 151://ǰ��
			go(60,300);
			Mark=152;				
			break;
			
		
		case 152://��ת
			turn_left(95);
			Mark=155;				
			break;
			
		case 155://Ѱ��
			go_track(60);
			Mark=156;				
			break;
		
		case 156://ǰ��
			go(60,300);
			Mark=157;				
			break;
		
		case 157://��ת
			turn_left(95);
			Mark=158;				
			break;	
			
		case 158://Ѱ��
			go_track(60);
//		  SYN_TTS("�����������");
			Mark=159;			
			break;	
		
		case 159://�嵽����F2ǰ
			go(60,1200);
//		   SYN_TTS("���ȥ��");
			Mark=160;			
			break;
		
		case 160://Ѱ��
			go_track(60);
			Mark=161;//16			
			break;
			
		case 161://ǰ��
			go(60,300);
			Mark=162;				
			break;	
			
		case 162://��ת
			turn_left(95);
			Mark=163;				
			break;	
			
		case 163:
			go_track_mp(30,1000);             //ѭ��ʹ������
			Mark=164;
			break;
			
		case 164:                          //���˽�����
			back(30,2100);
			Mark=165;
			break;	
			
		case 165:
			garage_Tx_Buff[3]=GARAGE_NO;//���⵽���һ��
			garage_Tx_Buff[6]=(garage_Tx_Buff[2]+garage_Tx_Buff[3]+garage_Tx_Buff[4]+garage_Tx_Buff[5])%256;
			Send_ZigbeeData_To_Fifo(garage_Tx_Buff,8);
			Mark=166;
			break;
		
		case 166:                          //���˽�����
			Send_ZigbeeData_To_Fifo(WUXIAN_tx_Buff,8);//���߳��
			delay_ms(200);
			Send_ZigbeeData_To_Fifo(jishioff_tx_Buff,8);//��ʱ����
		  Full_Enable=0;
			Mark=0;
			break;
	
		default:	
			break;		
			
		
	}
}


/**
�������ܣ�����ZigBee����
��    ��: ��
�� �� ֵ����
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
�������ܣ�ZigBee���ݼ��
��    ������
�� �� ֵ����
*/
void Can_ZigBeeRx_Check(void)
{
	if(Zigbee_Rx_flag)
	{
		if(gt_get_sub(canu_zibe_rxtime) == 0)
		{
			if(Zigb_Rx_Buf[1] == 0x03) 			// ��բ 
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
			else if((Zigb_Rx_Buf[1]==0x0E)&&(Zigb_Rx_Buf[2]==0x01))	//��ͨ�Ʊ�־��					
			{
				Stop_Flag = Zigb_Rx_Buf[4] ;	  						// 0x07����ʶ��ģʽ 0x08 δ����ʶ��ģʽ
			} 
			else if((Zigb_Rx_Buf[1]==0x0D)&&(Zigb_Rx_Buf[2]==0x03)) //���峵���־��
			{
				if(Zigb_Rx_Buf[3] == 0x01)							//��ȡ���峵�⵱ǰ����
				{
					switch(Zigb_Rx_Buf[4])							
					{
						case 1:
							Stop_Flag = 0x09;						//��һ��
							break;
						case 2:
							Stop_Flag = 0x0A;						//�ڶ���
							break;
						case 3:
							Stop_Flag = 0x0B;						//������
							break;
						case 4:
							Stop_Flag = 0x0C;						//���Ĳ�
							break;
					}
				} 
				else if(Zigb_Rx_Buf[3] == 0x02) 					//��ȡ���峵�⵱ǰ����״̬
				{
					if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x11;							//ǰ����ⴥ���������ⴥ��
					} 
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x22;							//ǰ�����δ������������δ����
					}
					else if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x12;							//ǰ����ⴥ����������δ����
					}
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x21;							//ǰ�����δ�����������ⴥ��
					}
				}
			}
			else if((Zigb_Rx_Buf[0] == 0x55)&&(Zigb_Rx_Buf[1] == 0x02))		//���شӳ�����
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
			else if(Zigb_Rx_Buf[1]==0x06)	 //������������
			{
				if(Zigb_Rx_Buf[2]==0x01)
				{
						Stop_Flag=Zigb_Rx_Buf[3] ;	  // ����оƬ״̬����
				}			
			}
			Zigbee_Rx_flag = 0;
		}	
	}
}


/** ��δʹ��
�������ܣ�����ѭ���ϴ�����ʱ��
��    ������
�� �� ֵ����
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
�������ܣ�CAN��ѯ�����ͽ��ռ��
��    ������
�� �� ֵ����
*/
void Canuser_main(void)
{
	CanP_Host_Main();
	CanP_CanTx_Check();				//CAN���߷������ݼ��
}



//end file

