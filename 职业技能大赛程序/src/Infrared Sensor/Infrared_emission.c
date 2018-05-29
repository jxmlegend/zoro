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
	     control_send=0;  //先发送0.56ms的38KHZ红外波（即编码中0.56ms的高电平）
	 	 Delay560us();
	  //停止发送红外信号（即编码中的低电平）
		if(irdata&1)      //判断最低位为1还是0。   低位先发送！！
	 	 { control_send=1;  //1为宽电平，1.693ms
	 	  Delay1693us();}
		else {control_send=1;  //0为窄电平，0.56ms
	 	      Delay560us();}
	     irdata=irdata>>1;     
    }
}

void SendIRdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
   //发送9ms的起始码，高电平有38KHZ载波
	control_send=0; 
    Delay9ms();
   //发送4.5ms的结果码，低电平无38KHZ载波
  control_send=1; 
	Delay4500us();
	
	//发送十六位地址
	SendIRdata_BYTE(g_iraddr1);
	uart_senddata(g_iraddr1);
	SendIRdata_BYTE(~g_iraddr1);
	uart_senddata(~g_iraddr1);
    //发送8位数据
	SendIRdata_BYTE(p_irdata);
	uart_senddata(p_irdata);
    //发送8位数据的反码
	SendIRdata_BYTE(~p_irdata);
	uart_senddata(~p_irdata);
   
  control_send=0; 
	   Delay560us();
	control_send=1; 
} 

void SendENDdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
   //发送9ms的起始码，高电平有38KHZ载波
	control_send=0; 
    Delay9ms();
   //发送4.5ms的结果码，低电平无38KHZ载波
  control_send=1; 
	Delay4500us();
	
	//发送十六位地址
	SendIRdata_BYTE(g_iraddr1);
	SendIRdata_BYTE(g_iraddr1);
    //发送8位数据
	SendIRdata_BYTE(p_irdata);
    //发送8位数据的反码
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
				temp_data=(uchar)(wsd&0x00ff);//温度
				if(temp_data==0x00)
				temp_data=0x62;
		    SendIRdata(ir_adress,temp_data);
			  Delay500ms();	//@11.0592MHz
			}
} 
 
 
