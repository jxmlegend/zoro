#include "esp8266.h"

extern u8 U1_Rec_Buff[200];
extern u8 U1_Rev_Done;
extern u8 U1_Rev_Count;

	//初始化esp8266模块
//返回值:0,成功;1,失败.
u8 esp8266_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	while(retry--)
	{
		U1_printf("AT");		//发送AT测试指令
		for(t=0;t<10;t++) 			//最长等待200ms,来接收HC05模块的回应
		{
			delay_ms(5);
			if(U1_Rev_Done)break;
		}		
		if(U1_Rev_Done)	//接收到一次数据了
		{
			U1_Rev_Done=0;U1_Rev_Count=0;		 
			if(strstr((const char *)U1_Rec_Buff,"OK"))
			{				
				break;
			}			
		}			    		
	}		    
	if(retry==0)temp=1;	//检测失败
	else temp=0;
	for(t=0;t<200;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}

//复位esp8266模块的角色
//返回值:0,成功;1,失败;0XFF,获取失败.							  
void esp8266_RST(void)
{	
	u8 t;
	U1_printf("AT+RST");		//发送AT测试指令
	delay_ms(200);	
	for(t=0;t<200;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
} 	


u8 esp8266_cmd(u8 *cmd,u8 waittime)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	while(retry--)
	{
		U1_printf(cmd);		//发送AT测试指令
		for(t=0;t<waittime;t++) 			//最长等待200ms,来接收HC05模块的回应
		{
			delay_ms(1);
			if(U1_Rev_Done)break;
		}		
		if(U1_Rev_Done)	//接收到一次数据了
		{
			U1_Rev_Done=0;U1_Rev_Count=0;		 
			if(strstr((const char *)U1_Rec_Buff,"OK"))
			{				
				break;
			}			
		}			    		
	}		    
	if(retry==0)temp=1;	//检测失败
	else temp=0;
	for(t=0;t<200;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}
