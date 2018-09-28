		 
#include "hc05.h" 
 
#define HC05_KEY_1  GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define HC05_KEY_0  GPIO_ResetBits(GPIOB,GPIO_Pin_10)
//////////////////////////////////////////////////////////////////////////////////	 
extern u8 U1_Rec_Buff[50];
extern u8 U1_Rev_Done;
extern u8 U1_Rev_Count;

////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��ATK-HC05ģ��
//����ֵ:0,�ɹ�;1,ʧ��.
u8 HC05_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   /* ����GPIOʱ�� */

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;		//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* ��ʼ��GPIO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;		//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //�����������ģʽ
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* ��ʼ��GPIO */
	
	while(retry--)
	{
		HC05_KEY_1;					//KEY�ø�,����ATģʽ
		delay_ms(10);
		U1_printf("AT\r\n");		//����AT����ָ��
		HC05_KEY_0;					//KEY����,�˳�ATģʽ
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
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;	 
}	 
//��ȡATK-HC05ģ��Ľ�ɫ
//����ֵ:0,�ӻ�;1,����;0XFF,��ȡʧ��.							  
u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY_1;					//KEY�ø�,����ATģʽ
		delay_ms(10);
		U1_printf("AT+ROLE?\r\n");	//��ѯ��ɫ
		HC05_KEY_0;					//KEY����,�˳�ATģʽ
		for(t=0;t<10;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			delay_ms(5);
			if(U1_Rev_Done)break;
		}		
		
		if(U1_Rev_Done)	//���յ�һ��������
		{
			U1_Rev_Done=0;
			if(U1_Rec_Buff[0]=='+')//���յ���ȷ��Ӧ����
			{
				temp=U1_Rec_Buff[6]-'0';//�õ�����ģʽֵ
				break;
			}
		}		
	}
	if(retry==0)temp=0XFF;//��ѯʧ��.
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;
} 							   
//ATK-HC05��������
//�˺�����������ATK-HC05,�����ڽ�����OKӦ���ATָ��
//atstr:ATָ�.����:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"���ַ���
//����ֵ:0,���óɹ�;����,����ʧ��.							  
u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY_1;					//KEY�ø�,����ATģʽ
		delay_ms(10);
		U1_printf(atstr);	//����AT�ַ���
		HC05_KEY_0;					//KEY����,�˳�ATģʽ
		for(t=0;t<20;t++) 			//��ȴ�100ms,������HC05ģ��Ļ�Ӧ
		{
			if(U1_Rev_Done)break;
			delay_ms(5);
		}		
		if(U1_Rev_Done)	//���յ�һ��������
		{
			U1_Rev_Done=0;		 
			if(U1_Rec_Buff[0]=='O')//���յ���ȷ��Ӧ����
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;//����ʧ��.
	for(t=0;t<50;t++)
	{
		U1_Rec_Buff[t]=0;
	}U1_Rev_Count=0;
	return temp;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////















