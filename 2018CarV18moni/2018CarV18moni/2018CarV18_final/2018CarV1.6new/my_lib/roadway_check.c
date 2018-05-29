#include "stm32f4xx.h"
#include "CanP_Hostcom.h"
#include "delay.h"
#include "roadway_check.h"
#include "cba.h"
#include "Timer.h"

uint8_t wheel_L_Flag =0,wheel_R_Flag = 0;
uint8_t wheel_Nav_Flag = 0;
uint8_t Go_Flag = 0,Back_Flag = 0;
uint8_t Track_Flag = 0;
uint8_t Track_mp_Flag = 0;
uint8_t Line_Flag = 0;
uint16_t count = 0;

uint8_t Stop_Flag = 0;
int LSpeed = 0,RSpeed = 0;
int Car_Spend = 0;
uint16_t temp_MP = 0;
uint16_t temp_Nav = 0;

uint8_t Wheel_flag = 0;

extern uint8_t  delay_Left_flag;
extern uint8_t  delay_Right_flag;	
extern uint8_t  delay_Left_flag_old;
extern uint8_t  delay_Right_flag_old;	

extern uint8_t premise_flag;
u32 wheel_time=0;
u16 set_wheel_time;
//_________________________________________________________
int16_t Roadway_cmp;
extern int16_t CanHost_Mp;

void Track(uint8_t gd);
void Track_mp(uint8_t gd,u16 mp_value);
/*
	����ͬ��
**/
void Roadway_mp_syn(void)
{
	Roadway_cmp = CanHost_Mp;
}

/*
	���̻�ȡ
**/
uint16_t Roadway_mp_Get(void)
{
	uint32_t ct;
	if(CanHost_Mp > Roadway_cmp)
		ct = CanHost_Mp - Roadway_cmp;
	else
		ct = Roadway_cmp - CanHost_Mp;
	if(ct > 0x8000)
		ct = 0xffff - ct;
	return ct;	
}

//_______________________________________________________________



//_________________________________________________________
uint16_t Roadway_Navig;
extern uint16_t CanHost_Navig;

/*
	�Ƕ�ͬ��
**/
void Roadway_nav_syn(void)
{
	Roadway_Navig = CanHost_Navig;
}

/*
	��ȡ�ǶȲ�ֵ
**/
uint16_t Roadway_nav_Get(void)
{
	uint16_t ct;
	if(CanHost_Navig > Roadway_Navig)
		ct = CanHost_Navig - Roadway_Navig;
	else
		ct = Roadway_Navig - CanHost_Navig;
	while(ct >= 36000)
		ct -= 36000;
	return ct;
}

//_______________________________________________________________

void Roadway_Flag_clean(void)
{
	wheel_L_Flag =0;wheel_R_Flag = 0;
	Go_Flag = 0;Back_Flag = 0;
	Track_Flag = 0;Track_mp_Flag = 0;
	Stop_Flag = 0;
	temp_MP = 0;
}

/**
	ǰ�����
*/
void Go_and_Back_Check(void)
{	
	if(Go_Flag == 1)
	{
		if(temp_MP <= Roadway_mp_Get())
		{
			Go_Flag = 0;
			Stop_Flag = 3;
			Send_UpMotor(0,0);
		}
	} 
	else if(Back_Flag == 1)
	{
		if(temp_MP <= Roadway_mp_Get())
		{
			Back_Flag = 0;
			Stop_Flag=3;
			Send_UpMotor(0,0);
		}
	}
}

uint8_t Roadway_GoBack_Check(void)
{
	return ((Go_Flag == 0)&&(Back_Flag == 0)&&(Track_Flag == 0)&&(Track_mp_Flag == 0)&&(wheel_L_Flag == 0)&&(wheel_R_Flag == 0))? 1:0;
	
}

/**
	�Ƕȼ��ת��
*/
void wheel_Nav_check()
{ 	
	uint16_t Navig_Value = 0;
	uint16_t Mp_Value = 0;
	
	if(wheel_Nav_Flag)
	{
		Navig_Value = Roadway_nav_Get();
		Mp_Value = Roadway_mp_Get();
		
//		if(Navig_Value  >= temp_Nav) 	//����ָ���Ƕ�
//		{	
//			wheel_Nav_Flag = 0;
//			Wheel_flag=0;
//			Stop_Flag=2;
//			Send_UpMotor(0,0);
//		} 
//		else 
		if(Mp_Value >=   temp_Nav)
		{
			wheel_Nav_Flag = 0;
			Wheel_flag=0;
			Stop_Flag=2;
			Send_UpMotor(0,0);
		}
	}
}


/**
	���̼��ת��
*/
void wheel_mp_check()
{ 	
	uint16_t Track_Value = 0;
	uint16_t Mp_Value = 0;
	if(wheel_L_Flag == 1)
	{
		Track_Value = Get_Host_UpTrack(TRACK_H8);
		Mp_Value = Roadway_mp_Get();
		if(Mp_Value > 5500)				  //�������50CM
		{
			wheel_L_Flag = 0;
			Wheel_flag=0;
			Stop_Flag=2;
		    Send_UpMotor(0,0);
		}
		else
		{
			if(!((Track_Value &0X18)&(Track_Value &0X08))) //�ҵ�ѭ���ߣ�ֹͣ
			{	
				if(Wheel_flag)
				{
					wheel_L_Flag = 0;
					Wheel_flag=0;
					Stop_Flag=2;
					Send_UpMotor(0,0);
				}
			}
			else if( Track_Value == 0Xff) 
				Wheel_flag=1;
		}
	} 
	else if(wheel_R_Flag == 1)
	{
		Track_Value = Get_Host_UpTrack(TRACK_H8);
		if(Mp_Value > 550)				  //�������50CM
		{
			wheel_R_Flag=0;
			Wheel_flag=0;
			Stop_Flag=2;
		    Send_UpMotor(0,0);
		}
		else
		{
		 if(!((Track_Value &0X18)&(Track_Value &0X10))) //�ҵ�ѭ���ߣ�ֹͣ
			{	
				if(Wheel_flag)
				{
					wheel_R_Flag=0;
					Wheel_flag=0;
					Stop_Flag=2;
					Send_UpMotor(0,0);
				}
			}
			else if( Track_Value == 0Xff) 
				Wheel_flag=1;	
		}
	}
	
}

//��ʱת��
void delay_LR(void)
{
	if((delay_Left_flag_old==0)&&(delay_Left_flag))
	{
	    wheel_time=gt_get();
	}
	if(delay_Left_flag)
	{		
		delay_Left_flag_old=delay_Left_flag;
				
		if((gt_get()-wheel_time)>set_wheel_time)
		{
			Send_UpMotor(0,0);				
			Stop_Flag=25;
			delay_Left_flag=0;	delay_Left_flag_old=0;set_wheel_time=0;
		}			
	}
	//----------------------------------------------------
	if((delay_Right_flag_old==0)&&(delay_Right_flag))
	{
	    wheel_time=gt_get();
	}
	if(delay_Right_flag)
	{		
		delay_Right_flag_old=delay_Right_flag;
		
		if((gt_get()-wheel_time)>set_wheel_time)
		{
			Send_UpMotor(0,0);
			Stop_Flag=26;
			delay_Right_flag=0;	delay_Right_flag_old=0;	set_wheel_time=0;
		}			
	}
}

/**
	ѭ�����
*/
void Track_Check()
{	
	
	if(Track_Flag == 1)
	{
		Track(Get_Host_UpTrack(TRACK_H8));
	}	
}
/**
	����ѭ�����
*/
void Track_mp_Check()
{	
	
	if(Track_mp_Flag == 1)
	{
		Track_mp(Get_Host_UpTrack(TRACK_H8),1);
	}	
}
void Roadway_Check(void)
{
	Go_and_Back_Check();
	wheel_mp_check();
	wheel_Nav_check();
	delay_LR();
	Track_Check();
	Track_mp_Check();
}
	

/***************************************************************
** ���ܣ�     ������ƺ���
** ������	  L_Spend����������ٶ�
**            R_Spend����������ٶ�
** ����ֵ��   ��	  
****************************************************************/
void Control(int L_Spend,int R_Spend)
{
	if(L_Spend>=0)
	{	
		if(L_Spend>100)L_Spend=100;if(L_Spend<5)L_Spend=5;		//�����ٶȲ���
	}
	else 
	{
		if(L_Spend<-100)L_Spend= -100;if(L_Spend>-5)L_Spend= -5;     //�����ٶȲ���
	}	
	if(R_Spend>=0)
	{	
		if(R_Spend>100)R_Spend=100;if(R_Spend<5)R_Spend=5;		//�����ٶȲ���
	}
	else
	{	
		if(R_Spend<-100)R_Spend= -100;if(R_Spend>-5)R_Spend= -5;		//�����ٶȲ���		
	}
	Send_UpMotor(L_Spend ,R_Spend);	
}

/***************************************************************
** ���ܣ�    ������ѭ������
** ������	  �޲���
** ����ֵ��   ��
//������ѭ������mp_value��Ϊ0������Ϊ�ǰ�����ѭ�������ӳ���ֹͣ������
//��mp_value=0,��Ϊԭѭ����������
//ʹ��ʱ����TRACK	ǰ������ͬ��������ͬʱmp_valueҪ��ֵ
****************************************************************/
void Track_mp(uint8_t gd,u16 mp_value)
{
	
	if( gd == 0x00)	//ѭ����ȫ�� ֹͣ
	{	
		Track_mp_Flag=0;
		Stop_Flag=1;
		Send_UpMotor(0,0);
	}
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7)//1���м�3/4��������⵽���ߣ�ȫ������
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}

		if(Line_Flag!=2)
		{		

			if(gd==0XF3||gd==0XF7||gd==0XFB) //2���м�4��3��������⵽���ߣ�΢�ҹ�	
			{ 
				LSpeed=Car_Spend+30;
				RSpeed=Car_Spend-30;
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3���м�3��2��������⵽���ߣ���΢�ҹ�		
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4���м�2��1��������⵽���ߣ�ǿ�ҹ�
			{ 
				LSpeed = Car_Spend+50;
				RSpeed = Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5�����ұ�1��������⵽���ߣ���ǿ�ҹ�			
			{ 
				 LSpeed = Car_Spend+60;
				 RSpeed = Car_Spend-120;			
				 Line_Flag=1;
			}
		}
		if(Line_Flag!=1)
		{
			if(gd==0XCF)//6���м�6��5��������⵽���ߣ�΢���			
			{ 
				 RSpeed = Car_Spend+30;
				 LSpeed = Car_Spend-30;
				 Line_Flag=0;
			} 
				else if(gd==0X9F||gd==0XEF||gd==0XDF)//7���м�7��6��������⵽���ߣ���΢���		 
			{ 
				 RSpeed = Car_Spend+40;
				 LSpeed = Car_Spend-60;
				 Line_Flag=0;
			} 			  
			else if(gd==0X3F||gd==0XBF)//8���м�8��7��������⵽���ߣ�ǿ���		 
			{ 
				 RSpeed = Car_Spend+50;
				 LSpeed = Car_Spend-90;
				 Line_Flag=0;
			} 
			else if(gd==0X7F)//9������8��������⵽���ߣ���ǿ���		 	
			{ 
				 RSpeed = Car_Spend+60;
				 LSpeed = Car_Spend-120;
				 Line_Flag=2;
			}			
		}
		if(gd==0xFF)   //ѭ����ȫ��
		{
			if(count>1000)
			{
					count=0;
					Send_UpMotor(0,0);
					Track_mp_Flag=0;
					if(Line_Flag ==0) 
					{Stop_Flag=4;}
			}	
			else 
				count++;				
		}
		else 
			count=0;
	}
	
	if(mp_value) //������ѭ������mp_value��Ϊ0������Ϊ�ǰ�����ѭ�������ӳ���ֹͣ������ʹ��ʱ����TRACK	ǰ������ͬ����������
	{
		if(temp_MP <= Roadway_mp_Get())
		{
			Track_mp_Flag=0;
			Stop_Flag=15;
			Send_UpMotor(0,0);
		}
	}
	
	if(Track_mp_Flag != 0)
	{
		static uint32_t gt_save = 0;
		uint32_t gt_temp;
		gt_temp = gt_get();
		if((gt_save + 2) < gt_temp)
		{
			Control(LSpeed,RSpeed);
			gt_save = gt_temp;
		}
	}
	else
	{
		Send_UpMotor(0,0);
	}
}
/***************************************************************
** ���ܣ�     ѭ������
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void Track(uint8_t gd)
{	
	if( gd == 0x00)	//ѭ����ȫ�� ֹͣ
	{	
		Track_Flag=0;
		Stop_Flag=1;
		Send_UpMotor(0,0);
	}
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7)//1���м�3/4��������⵽���ߣ�ȫ������
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}

		if(Line_Flag!=2)
		{		

			if(gd==0XF3||gd==0XF7||gd==0XFB) //2���м�4��3��������⵽���ߣ�΢�ҹ�	
			{ 
				LSpeed=Car_Spend+30;
				RSpeed=Car_Spend-30;
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3���м�3��2��������⵽���ߣ���΢�ҹ�		
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4���м�2��1��������⵽���ߣ�ǿ�ҹ�
			{ 
				LSpeed = Car_Spend+50;
				RSpeed = Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5�����ұ�1��������⵽���ߣ���ǿ�ҹ�			
			{ 
				 LSpeed = Car_Spend+60;
				 RSpeed = Car_Spend-120;			
				 Line_Flag=1;
			}
		}
		if(Line_Flag!=1)
		{
			if(gd==0XCF)//6���м�6��5��������⵽���ߣ�΢���			
			{ 
				 RSpeed = Car_Spend+30;
				 LSpeed = Car_Spend-30;
				 Line_Flag=0;
			} 
				else if(gd==0X9F||gd==0XEF||gd==0XDF)//7���м�7��6��������⵽���ߣ���΢���		 
			{ 
				 RSpeed = Car_Spend+40;
				 LSpeed = Car_Spend-60;
				 Line_Flag=0;
			} 			  
			else if(gd==0X3F||gd==0XBF)//8���м�8��7��������⵽���ߣ�ǿ���		 
			{ 
				 RSpeed = Car_Spend+50;
				 LSpeed = Car_Spend-90;
				 Line_Flag=0;
			} 
			else if(gd==0X7F)//9������8��������⵽���ߣ���ǿ���		 	
			{ 
				 RSpeed = Car_Spend+60;
				 LSpeed = Car_Spend-120;
				 Line_Flag=2;
			}			
		}
		if(gd==0xFF)   //ѭ����ȫ��
		{
			if(count>1000)
			{
					count=0;
					Send_UpMotor(0,0);
					Track_Flag=0;
					if(Line_Flag ==0) 
					{Stop_Flag=4;}
			}	
			else 
				count++;				
		}
		else 
			count=0;
	}
	
	if(Track_Flag != 0)
	{
		static uint32_t gt_save = 0;
		uint32_t gt_temp;
		gt_temp = gt_get();
		if((gt_save + 2) < gt_temp)
		{
			Control(LSpeed,RSpeed);
			gt_save = gt_temp;
		}
	}
	else
	{
		Send_UpMotor(0,0);
	}
}
//______________________________________________


void roadway_check_TimInit(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM_InitStructure.TIM_Period = arr;
	TIM_InitStructure.TIM_Prescaler = psc;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM9,&TIM_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM9, ENABLE);
}


void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
	{
		Roadway_Check();								//·�����
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
}


//end file



