#include "public.h"
#include "usart.h"
#include "systick.h"
#include "beep.h"
#include "led.h"
#include "time.h"
#include "pwm.h"
#include "Track.h"
#include "hc05.h"
#include <string.h>

extern u8 U1_Rev_Count;
extern u8 U1_Rev_Done;
extern u8 U1_Rec_Buff[50];

u16 speed=10000;

void BluetoothCar_cmd(void);
/****************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
int main()
{		  
	u8 flag,soure;
	Track_init();
	pwm_init();
	time_init();
	LED_Init();
	BEEP_Init();
	usart_init();
	TIM_SetCompare3(TIM3,1500);
	
	flag=HC05_Init();
	soure=HC05_Get_Role();
	HC05_Set_Cmd("AT+RESET\r\n");
	while(1)
	{
		BluetoothCar_cmd();
	}		
}

void BluetoothCar_cmd()
{
	u8 i;
	if(U1_Rev_Done)
	{
		if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == 'B') && (U1_Rec_Buff[2] == 'U') 
						&& (U1_Rec_Buff[3] == 'P') && (U1_Rec_Buff[4] == 'D') && (U1_Rec_Buff[5] == '>'))//前进
		{
			straight();
			TIM_SetCompare1(TIM3,speed);
			TIM_SetCompare2(TIM3,speed);
			TIM_SetCompare3(TIM3,1500);
		}    
		if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == 'B') && (U1_Rec_Buff[2] == 'D') 
				&& (U1_Rec_Buff[3] == 'N') && (U1_Rec_Buff[4] == 'D') && (U1_Rec_Buff[5] == '>'))//后退
		{
			turn_back();
			TIM_SetCompare1(TIM3,speed);
			TIM_SetCompare2(TIM3,speed);
			TIM_SetCompare3(TIM3,1500);
		} 

	 if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == 'B') && (U1_Rec_Buff[2] == 'L') 
				&& (U1_Rec_Buff[3] == 'T') && (U1_Rec_Buff[4] == 'D') && (U1_Rec_Buff[5] == '>'))//左转
		{
			turn_left();
			TIM_SetCompare1(TIM3,speed);
			TIM_SetCompare2(TIM3,speed);
			TIM_SetCompare3(TIM3,1200);
		} 
	 if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == 'B') && (U1_Rec_Buff[2] == 'R') 
				&& (U1_Rec_Buff[3] == 'T') && (U1_Rec_Buff[4] == 'D') && (U1_Rec_Buff[5] == '>'))//右转
		{
			turn_right();
			TIM_SetCompare1(TIM3,speed);
			TIM_SetCompare2(TIM3,speed);
			TIM_SetCompare3(TIM3,1800);
		} 
	 if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == 'B') && (U1_Rec_Buff[4] == 'U') && (U1_Rec_Buff[5] == '>'))//停止
		{
			turn_stop();
			TIM_SetCompare1(TIM3,0);
			TIM_SetCompare2(TIM3,0);
			TIM_SetCompare3(TIM3,1500);
		}
		if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == '+') && (U1_Rec_Buff[2] == '>'))
		{
			TIM_SetCompare1(TIM3,speed+=1000);
			TIM_SetCompare2(TIM3,speed+=1000);
			if(speed>20000)	speed=20000;
		}
		if(U1_Rec_Buff[0] == '<' && (U1_Rec_Buff[1] == '-') && (U1_Rec_Buff[2] == '>'))
		{
			TIM_SetCompare1(TIM3,speed-=1000);
			TIM_SetCompare2(TIM3,speed-=1000);
			if(speed<1)	speed=0;
		}
		if(strstr((const char *)U1_Rec_Buff,"HELLO_MCU!"))
		{
			U1_printf("HELLO_PC,I'm usart1 !\r\n");
		}
		if(strstr((const char *)U1_Rec_Buff,"LED_ON"))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
			GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		}
		if(strstr((const char *)U1_Rec_Buff,"LED_OFF"))
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
			GPIO_SetBits(GPIOE,GPIO_Pin_5);
		}
		if(strstr((const char *)U1_Rec_Buff,"BUZZ_ON"))
		{
			//GPIO_SetBits(GPIOB,GPIO_Pin_8);
			sound2(); 
		}
		if(strstr((const char *)U1_Rec_Buff,"BUZZ_OFF"))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
		}
		
		
		for(i=0;i<50;i++) 
			U1_Rec_Buff[i]=0;
		U1_Rev_Done=0;
	}
}



//		infrared();
//		delay_ms(10);
////		straight();
////		delay_ms(1000);
////		delay_ms(1000);
////		TIM_SetCompare3(TIM3,1500);
////		turn_left();
////		delay_ms(1000);
////		delay_ms(1000);
////		turn_right();
////		delay_ms(1000);
////		delay_ms(1000);
////		turn_back();
////		delay_ms(1000);
////		delay_ms(1000);
