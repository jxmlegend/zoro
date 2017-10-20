#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "AD.h"

u16 AD_integ(void);
int main(void)
{		
  u16 TIMCounter,votage;
	delay_init();	    	 //延时函数初始化	 
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();
	AD_init();
	 
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_SSD_BackLightSet(50);//调整背光
	LCD_Clear(BLACK);//调整背景色
	LCD_ShowString(60,170,200,16,16,"TIM cnt is:000000");	
	LCD_ShowString(60,190,200,16,16,"AD voltage is: 0.000V");	
	TIM3_Int_Init(50000,71);//1Mhz的计数频率，计数到10000为10ms
   	while(1)
	{
		A_H;B_H;//一开始让电容放电11
		delay_ms(400);
		
		/*A_L;B_L;//打开模拟开关s0开始充电00
		delay_ms(6);//等待充电完毕
		
		//开始反向积分打开定时器
		TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
		A_L;B_H;//10
	
		//判断电压是否翻转  
		while(flag==0);
		
		//停止计数读取计数值 
		TIMCounter = TIM_GetCounter(TIM3);
		TIM_Cmd(TIM3, DISABLE); //失能TIMx		
		TIM_SetCounter(TIM3,0);*/
		
		//将计数值转换成电压
//		votage=5.0*TIMCounter/6;
		TIMCounter = AD_integ();
		if(TIMCounter>0&&TIMCounter<3575)
			votage=0.61*TIMCounter+325;
		else if(TIMCounter>3575&&TIMCounter<5230)
			votage=0.61*TIMCounter+305;
		else if(TIMCounter>5230)
			votage=0.61*TIMCounter+350;
		printf("定时器的计数值为：%d\r\n",TIMCounter);
		//LCD显示计数值
		LCD_ShowxNum(148,170,TIMCounter,6,16,0x80);//显示计数值
		//LCD显示电压值
		LCD_ShowxNum(180,190,votage/1000,1,16,0x80);//显示电压值
		LCD_ShowxNum(196,190,votage%1000,3,16,0x80);//显示电压值
		LED0=!LED0;
		
	}	 
}	 
 
u16 AD_integ(void)
{
	u16 TIM_cnt;//定时器的计数值
	if(sex==1)//正电压
	{
		A_L;B_L;//打开模拟开关s0开始充电//11开始充电
		delay_ms(10);//等待充电完毕
		//开始反向积分打开定时器
		TIM_Cmd(TIM3, ENABLE);//1Mhz的计数频率，计数到10000为10ms
		A_L;B_H;//10
		
		//判断电压是否翻转   //用外部中断
		while(flag==0);
		
		//停止计数读取计数值 
		TIM_Cmd(TIM3, DISABLE); //失能TIMx
		
		TIM_cnt=TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3,0);
		LCD_ShowString(60,190,200,16,16,"AD voltage is:+0.000V");
		return TIM_cnt;
	}
	else//负电压
	{
		
		A_L;B_L;//打开模拟开关s0开始充电//11开始充电
		delay_ms(10);//等待充电完毕
		//开始反向积分打开定时器
		TIM_Cmd(TIM3, ENABLE);//1Mhz的计数频率，计数到10000为10ms
		A_H;B_L;//10
		
		//判断电压是否翻转
		while(flag==0);
		
		//停止计数读取计数值  
		TIM_Cmd(TIM3, DISABLE);  //失能TIMx
		
		TIM_cnt=TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3,0);
		LCD_ShowString(60,190,200,16,16,"AD voltage is:-0.000V");
		return TIM_cnt;
	}
}

