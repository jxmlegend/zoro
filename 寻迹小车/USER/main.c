#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "pwm.h"
#include "PID.h"
#include "remote.h"
#include "Track.h"
#include <math.h>

u8 flag,key_count,key=0;
u8 key_val[3];

float speed_Angle=30;//���õĽǶ�
extern pid_InitTypeDef pid;

void key_display(u8);
void Remote_display(void);

 int main(void)
 {	 
	delay_init();	    	 //��ʱ������ʼ��	  
	pwm_init();
	Track_init();

	while(1)
	{
		infrared();
		delay_ms(100);
	}
 }
 

void key_display(u8 key)
{
	switch(key)
	{	
//		case 162:str="CH-";flag=1;break;//POWER  
//		case 98:str="CH";flag=1;break;//UP    
//		case 2:str="RIGHT";flag=1;break;//PLAY 
//		case 226:str="CH+";flag=1;break;//ALIENTEK
		case 194:flag=10;break;//RIGHT
//		case 34:str="LEFT";flag=1;break;	//	  
//		case 224:str="VOL-";flag=1;break;//		  
//		case 168:str="VOL+";flag=1;break;//DOWN	   
		case 144:flag=11;break;//VOL+    
		case 104:flag=0;break;//1  
//		case 152:str="100+";flag=1;break;//2  
//		case 176:str="200+";flag=1;break;//3  
		case 48:flag=1;break;//4
		case 24:flag=2;break;//5
		case 122:flag=3;break;//6
		case 16:flag=4;break;//7					
		case 56:flag=5;break;//8
		case 90:flag=6;break;//9
		case 66:flag=7;break;//0
		case 74:flag=8;break;//ERROR 
		case 82:flag=9;break;//DELETE 
//		default :flag=1; break; 
	}
}

void Remote_display(void)
{
	u8 i=0,j=0;
	key=Remote_Scan();
	//�а�������
	if(key)
	{
		key_display(key);
		if(flag==10)//PLAY��
		{
			for(j=0;j<key_count;j++)
			{
				LCD_ShowNum(86+j*8,150,key_val[j],1,16);
			}	
			speed_Angle=key_val[0]*100+key_val[1]*10+key_val[2];//�û����õĽǶ�ֵ
			key_count=0;
		}
		if(flag==11)//EQ��
		{
			LCD_Fill(86,150,116+8*8,150+16,WHITE);	//���֮ǰ����ʾ
			LCD_Fill(86,170,116+8*8,170+16,WHITE);	//���֮ǰ����ʾ
			key_count=0;
		}
		if((flag>=0)&&(flag<=9))//���ּ�
		{
			if(key_count==3) key_count=0;
			key_val[key_count++]=flag;
			for(i=0;i<key_count;i++)
			{
				LCD_ShowNum(86+i*8,170,key_val[i],1,16);
			}
		}
	}
	else delay_ms(10);
//	while(Remote_Scan());
//		delay(10);
	while(Remote_Scan());
	delay_ms(10);
}

