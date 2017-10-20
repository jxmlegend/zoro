#include "led.h"

#include "key.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "beep.h"
#include "lcd.h"
#include "remote.h"
#include "timer.h"
#include <math.h>
#include <stdlib.h>
#include "delay.h"
u16 gTime_1us;
u32 gTime_1ms;
extern u8 direction_flag1,direction_flag2;
extern u8 ball_cnt1,ball_cnt2;
extern u16 TIM_cnt1,TIM_cnt2,TIM_cnt3;
extern u8 swing1,swing2;
extern u16 v1,v2;
extern  u16 a,angle;

u8 h_flag;//红外状态
u8 cnt;//球的计数值
u8 speed_mode=1;//模式
//u8 key_val;
u8 flag1,flag2;
u8 start,end;
u16 systick_0,systick_1;
void mode1(void);
void mode2(void);
void mode3(void);
void key_display(void);
void empty(void);

 int main(void)
  {		
		  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	EXTIX_Init();		 	//外部中断初始化
	SysTick_Init();	    	 //延时函数初始化
	//LCD_Init();
	Remote_Init();			//红外接收初始化
	
	//定时器初始化
	TIM2_Int_Init(49999,71);
	TIM3_Int_Init(49999,359);//1Mhz的计数频率，计数到10000为10ms 
	TIM5_Int_Init(49999,359);
	POINT_COLOR=BLUE;	  
//	LCD_ShowString(30,90,200,16,16,"Ball mode is:");//模式
//	LCD_ShowString(30,110,200,16,16,"Ball aspect is:");//方向
//	LCD_ShowString(30,130,200,16,16,"Ball Num is:");//数量	      
//	LCD_ShowString(30,150,200,16,16,"Real Angle is:");//角度
//	LCD_ShowString(30,170,200,16,16,"key is:");//键值
//	LCD_ShowString(30,190,200,16,16,"period is:");//周期
	while(1)
	{	    
//		key_display();
		switch(speed_mode)
		{
			case 1:mode1();break;
			case 2:mode2();break;
			case 3:mode3();break;
			default : mode1();break;
		}
		if(start) 
		{
			systick_0= gTime_1ms;
		}
		if(end&&start)
		{
			systick_1= gTime_1ms;
			if(systick_1<systick_0)
				TIM_cnt3= 50000-systick_0+systick_1;
			else
				TIM_cnt3= systick_1-systick_0;
		}
	}
 }

 void mode1(void)//普通放入模式--显示
 {
	
	//模式
	LCD_ShowNum(134,90,1,1,16);
	//方向
	if(flag1==0)//A-B
	{
		LCD_ShowString(150,110,200,16,16,"10");
	}
	else//B-A
	{
		LCD_ShowString(150,110,200,16,16,"01");
	}
	//数量
//	cnt=(ball_cnt1+ball_cnt2)/4;
	cnt=ball_cnt1;
	LCD_ShowxNum(126,130,cnt,2,16,0);
	//角度
	a=(v2-v1)*10000/TIM_cnt3;
	LCD_ShowxNum(30,270,a,3,16,0);
	angle=asin(a/0.00102);
	LCD_ShowxNum(142,150,angle,2,16,0);
	//显示
	LCD_ShowxNum(30,290,TIM_cnt3,6,16,0x80);
 }
 
 void mode2(void)//摆动模式--显示
 {
	u8 swing;
	LCD_ShowNum(134,90,2,1,16);
	
	swing=(swing1+swing2)/4;
	LCD_ShowxNum(110,190,swing,2,16,0x80);
 }
 
 void mode3(void)//快速放入模式--显示
 {
	LCD_ShowNum(134,90,3,1,16);	
 }
 void key_display(void)
{
	u8 key;
	key=Remote_Scan();
	if(key)
	{
		switch(key)
		{	
	//		case 162:str="CH-";flag=1;break;//POWER  
	//		case 98:str="CH";flag=1;break;//UP    
	//		case 2:str="RIGHT";flag=1;break;//PLAY 
	//		case 226:str="CH+";flag=1;break;//ALIENTEK
			case 194:h_flag=10;break;//RIGHT
	//		case 34:str="LEFT";flag=1;break;	//	  
	//		case 224:str="VOL-";flag=1;break;//		  
	//		case 168:str="VOL+";flag=1;break;//DOWN	   
			case 144:h_flag=11;empty();break;//VOL+    
			case 104:h_flag=0;break;//1  
	//		case 152:str="100+";flag=1;break;//2  
	//		case 176:str="200+";flag=1;break;//3  
			case 48:h_flag=1;speed_mode=1;break;//4
			case 24:h_flag=2;speed_mode=2;break;//5
			case 122:h_flag=3;speed_mode=3;break;//6
			case 16:h_flag=4;speed_mode=4;break;//7					
			case 56:h_flag=5;speed_mode=5;break;//8
			case 90:h_flag=6;speed_mode=6;break;//9
			case 66:h_flag=7;speed_mode=7;break;//0
			case 74:h_flag=8;speed_mode=8;break;//ERROR 
			case 82:h_flag=9;speed_mode=9;break;//DELETE 
	//		default :flag=1; break; 
		}
	}else Delay_ms(10);
	
}
 void empty(void)
 {
	//方向
	LCD_Fill(150,110,180+8*8,110+16,WHITE);	//清楚之前的显示 
	//数量
	LCD_Fill(126,130,156+8*8,130+16,WHITE);	//清楚之前的显示 
	//角度 
	LCD_Fill(142,150,172+8*8,150+16,WHITE);	//清楚之前的显示  
	//周期
	LCD_Fill(110,190,140+8*8,190+16,WHITE);	//清楚之前的显示 
}
// void key_display(u8 key)
//{
//	switch(key)
//	{	
////		case 162:str="CH-";flag=1;break;//POWER  
////		case 98:str="CH";flag=1;break;//UP    
////		case 2:str="RIGHT";flag=1;break;//PLAY 
////		case 226:str="CH+";flag=1;break;//ALIENTEK
//		case 194:h_flag=10;break;//RIGHT
////		case 34:str="LEFT";flag=1;break;	//	  
////		case 224:str="VOL-";flag=1;break;//		  
////		case 168:str="VOL+";flag=1;break;//DOWN	   
//		case 144:h_flag=11;break;//VOL+    
//		case 104:h_flag=0;break;//1  
////		case 152:str="100+";flag=1;break;//2  
////		case 176:str="200+";flag=1;break;//3  
//		case 48:h_flag=1;break;//4
//		case 24:h_flag=2;break;//5
//		case 122:h_flag=3;break;//6
//		case 16:h_flag=4;break;//7					
//		case 56:h_flag=5;break;//8
//		case 90:h_flag=6;break;//9
//		case 66:h_flag=7;break;//0
//		case 74:h_flag=8;break;//ERROR 
//		case 82:h_flag=9;break;//DELETE 
////		default :flag=1; break; 
//	}
//}

//void Remote_display(void)
//{
//	u8 i=0,j=0;
//	key=Remote_Scan();
//	//有按键按下
//	if(key)
//	{
//		key_display(key);
//		if(h_flag==10)//PLAY键
//		{
//			for(j=0;j<key_count;j++)
//			{
//				LCD_ShowNum(86+j*8,150,key_val[j],1,16);
//			}	
//			speed_mode=key_val[0]*100+key_val[1]*10+key_val[2];//用户设置的角度值
//			key_count=0;
//		}
//		if(h_flag==11)//EQ键
//		{
//			LCD_Fill(126,130,126+14*8,130+16,WHITE);	//清楚之前的显示数量
//			LCD_Fill(142,150,142+16*8,150+16,WHITE);	//清楚之前的显示角度
//			LCD_Fill(110,190,110+12*8,190+16,WHITE);	//清楚之前的显示周期
//			
//			key_count=0;
//		}
//		if((h_flag>=0)&&(h_flag<=9))//数字键
//		{
//			if(key_count==3) key_count=0;
//			key_val[key_count++]=h_flag;
//			for(i=0;i<key_count;i++)
//			{
//				LCD_ShowNum(86+i*8,170,key_val[i],1,16);
//			}
//		}
//	}
//	else delay_ms(10);
////	while(Remote_Scan());
////		delay(10);
//	while(Remote_Scan());
//	delay_ms(10);
//}
 
// void key_display(u8 key)
//{
//	switch(key)
//	{	
////		case 162:str="CH-";flag=1;break;//POWER  
////		case 98:str="CH";flag=1;break;//UP    
////		case 2:str="RIGHT";flag=1;break;//PLAY 
////		case 226:str="CH+";flag=1;break;//ALIENTEK
//		case 194:flag=10;break;//RIGHT
////		case 34:str="LEFT";flag=1;break;	//	  
////		case 224:str="VOL-";flag=1;break;//		  
////		case 168:str="VOL+";flag=1;break;//DOWN	   
//		case 144:flag=11;break;//VOL+    
//		case 104:flag=0;break;//1  
////		case 152:str="100+";flag=1;break;//2  
////		case 176:str="200+";flag=1;break;//3  
//		case 48:flag=1;break;//4
//		case 24:flag=2;break;//5
//		case 122:flag=3;break;//6
//		case 16:flag=4;break;//7					
//		case 56:flag=5;break;//8
//		case 90:flag=6;break;//9
//		case 66:flag=7;break;//0
//		case 74:flag=8;break;//ERROR 
//		case 82:flag=9;break;//DELETE 
////		default :flag=1; break; 
//	}
//}

//void Remote_display(void)
//{
//	key=Remote_Scan();
//	//有按键按下
//	if(key)
//	{
//		key_display(key);
//		if(flag==10)//PLAY键
//		{	
//			speed_mode=key_val;//用户设置的角度值
//		}
//		if(flag==11)//EQ键
//		{
//			LCD_Fill(126,130,126+14*8,130+16,WHITE);	//清楚之前的显示数量
//			LCD_Fill(142,150,142+16*8,150+16,WHITE);	//清楚之前的显示角度
//			LCD_Fill(110,190,110+12*8,190+16,WHITE);	//清楚之前的显示周期
//			
//		}
//		if((flag>=0)&&(flag<=9))//数字键
//		{
//			//if(key_count==3) key_count=0;
//			key_val=flag;
//			LCD_ShowNum(86,170,key_val,1,16);
//		}
//	}
//	else delay_ms(15);
////	while(Remote_Scan());
////		delay(10);
//	while(Remote_Scan());
//	delay_ms(10);
//}

