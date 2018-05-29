#include "HCSR04.h"
#include "uart.h"
#include "delay.h"


float distance(void)
{
	float dis;
	uint disbuff=0;
	uint x,y;
	Trig=1;
	delay_us(30);
	Trig=0;
	while(Echo==0);
	while(Echo==1)
	{		
		for(x=1;x>0;x--)
		for(y=113;y>0;y--);
		disbuff++;
	}	
	dis=disbuff*0.17;
	disbuff=0;
	return dis;
}


	