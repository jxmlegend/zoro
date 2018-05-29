#include "Infrared_emission.h"
#include "IR_delay.h"
#include "delay.h"
#include "dth.h"
#include "uart.h"

 void SendIRdata_BYTE(unsigned char irdata)
 {
 	 unsigned char i;
	 for(i=0;i<8;i++)
	 {	
	     control_send=0;  //�ȷ���0.56ms��38KHZ���Ⲩ����������0.56ms�ĸߵ�ƽ��
	 	 Delay560us();
	  //ֹͣ���ͺ����źţ��������еĵ͵�ƽ��
		if(irdata&1)      //�ж����λΪ1����0��   ��λ�ȷ��ͣ���
	 	 { control_send=1;  //1Ϊ���ƽ��1.693ms
	 	  Delay1693us();}
		else {control_send=1;  //0Ϊխ��ƽ��0.56ms
	 	      Delay560us();}
	     irdata=irdata>>1;     
    }
}

void SendIRdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
   //����9ms����ʼ�룬�ߵ�ƽ��38KHZ�ز�
	control_send=0; 
    Delay9ms();
   //����4.5ms�Ľ���룬�͵�ƽ��38KHZ�ز�
  control_send=1; 
	Delay4500us();
	
	//����ʮ��λ��ַ
	SendIRdata_BYTE(g_iraddr1);
	uart_senddata(g_iraddr1);
	SendIRdata_BYTE(~g_iraddr1);
	uart_senddata(~g_iraddr1);
    //����8λ����
	SendIRdata_BYTE(p_irdata);
	uart_senddata(p_irdata);
    //����8λ���ݵķ���
	SendIRdata_BYTE(~p_irdata);
	uart_senddata(~p_irdata);
   
  control_send=0; 
	   Delay560us();
	control_send=1; 
} 

void SendENDdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
   //����9ms����ʼ�룬�ߵ�ƽ��38KHZ�ز�
	control_send=0; 
    Delay9ms();
   //����4.5ms�Ľ���룬�͵�ƽ��38KHZ�ز�
  control_send=1; 
	Delay4500us();
	
	//����ʮ��λ��ַ
	SendIRdata_BYTE(g_iraddr1);
	SendIRdata_BYTE(g_iraddr1);
    //����8λ����
	SendIRdata_BYTE(p_irdata);
    //����8λ���ݵķ���
	SendIRdata_BYTE(p_irdata);
   
  control_send=0; 
	   Delay560us();
	control_send=1; 
} 

 void Multiple_sendIRdata(u8 count,u8 ir_adress,u8 dat)
{
			uchar i;
			for(i=0;i<count;i++)
			{
		    SendIRdata(ir_adress,dat);
				Delay_1ms(10);
//				SendENDdata(0xff,0xff);
				
			}
} 

void Multiple_send_Temp_IRdata(u8 count,u8 ir_adress)
{
			uchar i;
	    uchar temp_data=0x00;
	    uint wsd=0x0000;
			for(i=0;i<count;i++)
			{
				wsd=DHT11();
				temp_data=(uchar)(wsd&0x00ff);//�¶�
				if(temp_data==0x00)
				temp_data=0x62;
		    SendIRdata(ir_adress,temp_data);
			  Delay500ms();	//@11.0592MHz
			}
} 
 
 
