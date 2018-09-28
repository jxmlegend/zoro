#include "esp8266.h"

extern u8 U1_Rec_Buff[200];
extern u8 U1_Rev_Done;
extern u8 U1_Rev_Count;

	//��ʼ��esp8266ģ��
//����ֵ:0,�ɹ�;1,ʧ��.
u8 esp8266_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	while(retry--)
	{
		U1_printf("AT");		//����AT����ָ��
		for(t=0;t<10;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			delay_ms(5);
			if(U1_Rev_Done)break;
		}		
		if(U1_Rev_Done)	//���յ�һ��������
		{
			U1_Rev_Done=0;U1_Rev_Count=0;		 
			if(strstr((const char *)U1_Rec_Buff,"OK"))
			{				
				break;
			}			
		}			    		
	}		    
	if(retry==0)temp=1;	//���ʧ��
	else temp=0;
	for(t=0;t<200;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}

//��λesp8266ģ��Ľ�ɫ
//����ֵ:0,�ɹ�;1,ʧ��;0XFF,��ȡʧ��.							  
void esp8266_RST(void)
{	
	u8 t;
	U1_printf("AT+RST");		//����AT����ָ��
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
		U1_printf(cmd);		//����AT����ָ��
		for(t=0;t<waittime;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			delay_ms(1);
			if(U1_Rev_Done)break;
		}		
		if(U1_Rev_Done)	//���յ�һ��������
		{
			U1_Rev_Done=0;U1_Rev_Count=0;		 
			if(strstr((const char *)U1_Rec_Buff,"OK"))
			{				
				break;
			}			
		}			    		
	}		    
	if(retry==0)temp=1;	//���ʧ��
	else temp=0;
	for(t=0;t<200;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}
