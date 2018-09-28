		 
#include "hc05.h" 
 
#define HC05_KEY_1  GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define HC05_KEY_0  GPIO_ResetBits(GPIOB,GPIO_Pin_10)
//////////////////////////////////////////////////////////////////////////////////	 
extern u8 U1_Rec_Buff[50];
extern u8 U1_Rev_Done;
extern u8 U1_Rev_Count;

////////////////////////////////////////////////////////////////////////////////// 	   

//初始化ATK-HC05模块
//返回值:0,成功;1,失败.
u8 HC05_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   /* 开启GPIO时钟 */

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;		//选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* 初始化GPIO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;		//选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //设置推挽输出模式
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* 初始化GPIO */
	
	while(retry--)
	{
		HC05_KEY_1;					//KEY置高,进入AT模式
		delay_ms(10);
		U1_printf("AT\r\n");		//发送AT测试指令
		HC05_KEY_0;					//KEY拉低,退出AT模式
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
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}	 
//获取ATK-HC05模块的角色
//返回值:0,从机;1,主机;0XFF,获取失败.							  
u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY_1;					//KEY置高,进入AT模式
		delay_ms(10);
		U1_printf("AT+ROLE?\r\n");	//查询角色
		HC05_KEY_0;					//KEY拉低,退出AT模式
		for(t=0;t<10;t++) 			//最长等待200ms,来接收HC05模块的回应
		{
			delay_ms(5);
			if(U1_Rev_Done)break;
		}		
		
		if(U1_Rev_Done)	//接收到一次数据了
		{
			U1_Rev_Done=0;
			if(U1_Rec_Buff[0]=='+')//接收到正确的应答了
			{
				temp=U1_Rec_Buff[6]-'0';//得到主从模式值
				break;
			}
		}		
	}
	if(retry==0)temp=0XFF;//查询失败.
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;
} 							   
//ATK-HC05设置命令
//此函数用于设置ATK-HC05,适用于仅返回OK应答的AT指令
//atstr:AT指令串.比如:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"等字符串
//返回值:0,设置成功;其他,设置失败.							  
u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY_1;					//KEY置高,进入AT模式
		delay_ms(10);
		U1_printf(atstr);	//发送AT字符串
		HC05_KEY_0;					//KEY拉低,退出AT模式
		for(t=0;t<20;t++) 			//最长等待100ms,来接收HC05模块的回应
		{
			if(U1_Rev_Done)break;
			delay_ms(5);
		}		
		if(U1_Rev_Done)	//接收到一次数据了
		{
			U1_Rev_Done=0;		 
			if(U1_Rec_Buff[0]=='O')//接收到正确的应答了
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;//设置失败.
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////















