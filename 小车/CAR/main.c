#include <at89x51.h>
#define uchar unsigned char
#define uint unsigned int
#define IN P0
uchar pro_left,pro_right,i,j; //左右占空比标志

sbit left1=P2^0;
sbit left2=P2^1;
sbit right1=P2^3;
sbit right2=P2^2;   
sbit en1=P1^0;      
sbit en2=P1^1;  

uchar left_offset;  //偏左标志
uchar mid_offset;   //居中标志
uchar right_offset; //偏右标志
uchar stop;         //停车标志
void delay(uint z)
{
	uchar i;
	while(z--)
	{
		for(i=0;i<121;i++);
	}
}
void init()
{
	left_offset=0;  //初始化左偏标志
	mid_offset=1;   //初始化居中标志
	right_offset=0; //初始化右偏标志
	TMOD=0X01; 
	TH0=(65536-100)/256;
	TL0=(65536-100)%256;
	EA=1;
	ET0=1;
	TR0=1;
	en1=1;
	en2=1;
}
//使用中断做PWM发生器
void time0(void)interrupt 1
{
	i++;
	j++;
	if(i<=pro_right){en1=1;}
		 else en1=0;
	if(i==40) {en1=~en1;i=0;}
	if(j<=pro_left) {en2=1;}
		 else en2=0;
	if(j==40) {en2=~en2;j=0;}
	TH0=(65536-100)/256;
	TL0=(65536-100)%256;

}
void straight() //直行函数
{
	pro_right=20;
	pro_left=20;
	left1=0;
	left2=1;
	right1=0;
	right2=1;

}
void turn_left() //左转调整函数
{
	pro_right=30;
	pro_left=30;
	left1=1;
	left2=0;
	right1=0;
	right2=1;

}
void turn_right() //右转调整函数
{
	pro_right=30;
	pro_left=30;
	left1=0;
	left2=1;
	right1=1;
	right2=0;

}
void turn_back() //倒车函数
{
	left1=1;
	left2=0;
	right1=1;
	right2=0;
	pro_right=20;
	pro_left=20;

}
void turn_stop() //停车函数
{
	left1=0;
	left2=0;
	right1=0;
	right2=0;
	pro_right=0;
	pro_left=0;	 	
}
void infrared() //循迹函数
{
	uchar flag;

	if(IN==0xff) stop=1;
	else stop=0;

	while(!stop) //没有循迹停止，一直循迹下去
	{	
		switch(IN)
		{
			//居中状态
			case 0x10: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;	  //00010000
			case 0x08: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;	  //00001000
			case 0x18: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;   //00011000
			//偏右状态
			case 0x20: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //00100000
			case 0x40: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //01000000
			case 0x80: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //10000000
			case 0x30: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //00110000
			case 0x60: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //01100000
			case 0xc0: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //11000000
			
			//偏左状态
			case 0x04: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000100
			case 0x02: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000010
			case 0x01: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000001
			case 0x03: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00001100
			case 0x06: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000110
			case 0x0c: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000011

			case 0xFF: stop=1; break;//循到路口黑线 left_offset=1; right_offset=1;mid_offset=1;
			case 0x00: left_offset=0; right_offset=0;mid_offset=0;stop=0;break;//出界
			default: break;
		}
		if(stop)
		{
			turn_stop(); delay(5000);
		}
		else
		{
			TR0=1;
			if(left_offset==1)
				 {flag=1;}
			else
			if(right_offset==1)
				 {flag=2;}
			else
			if((left_offset==0)&(mid_offset==0)&(right_offset==0))	 //出界，倒车
				 {flag=3;stop=0;}
			else {flag=0;}											 //未知状态，直行
				 
			switch (flag)
			{
				case 0: straight();	  	break;
				case 1: turn_right();  	break;
				case 2: turn_left(); 	break;
				case 3: turn_back();  	break;	 			
				default:   		 		break;
			}
		} 
	}
}
void main(void)
{
	 init();	
	 infrared();
	 delay(2500);
	 turn_left() ;
	 delay(2500);
	 infrared();
	 delay(2500);
	 turn_right();
	 delay(2500);
	 delay(2500);
	 infrared();
	 delay(2500);
	 turn_left() ;
	 delay(2500);
	 infrared();
	 delay(2500);
	 turn_right();
	 delay(2500);
	 infrared();
	 delay(2500);	
}
