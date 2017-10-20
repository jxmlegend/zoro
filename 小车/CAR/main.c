#include <at89x51.h>
#define uchar unsigned char
#define uint unsigned int
#define IN P0
uchar pro_left,pro_right,i,j; //����ռ�ձȱ�־

sbit left1=P2^0;
sbit left2=P2^1;
sbit right1=P2^3;
sbit right2=P2^2;   
sbit en1=P1^0;      
sbit en2=P1^1;  

uchar left_offset;  //ƫ���־
uchar mid_offset;   //���б�־
uchar right_offset; //ƫ�ұ�־
uchar stop;         //ͣ����־
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
	left_offset=0;  //��ʼ����ƫ��־
	mid_offset=1;   //��ʼ�����б�־
	right_offset=0; //��ʼ����ƫ��־
	TMOD=0X01; 
	TH0=(65536-100)/256;
	TL0=(65536-100)%256;
	EA=1;
	ET0=1;
	TR0=1;
	en1=1;
	en2=1;
}
//ʹ���ж���PWM������
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
void straight() //ֱ�к���
{
	pro_right=20;
	pro_left=20;
	left1=0;
	left2=1;
	right1=0;
	right2=1;

}
void turn_left() //��ת��������
{
	pro_right=30;
	pro_left=30;
	left1=1;
	left2=0;
	right1=0;
	right2=1;

}
void turn_right() //��ת��������
{
	pro_right=30;
	pro_left=30;
	left1=0;
	left2=1;
	right1=1;
	right2=0;

}
void turn_back() //��������
{
	left1=1;
	left2=0;
	right1=1;
	right2=0;
	pro_right=20;
	pro_left=20;

}
void turn_stop() //ͣ������
{
	left1=0;
	left2=0;
	right1=0;
	right2=0;
	pro_right=0;
	pro_left=0;	 	
}
void infrared() //ѭ������
{
	uchar flag;

	if(IN==0xff) stop=1;
	else stop=0;

	while(!stop) //û��ѭ��ֹͣ��һֱѭ����ȥ
	{	
		switch(IN)
		{
			//����״̬
			case 0x10: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;	  //00010000
			case 0x08: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;	  //00001000
			case 0x18: left_offset=0; right_offset=0;mid_offset=1;stop=0;break;   //00011000
			//ƫ��״̬
			case 0x20: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //00100000
			case 0x40: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //01000000
			case 0x80: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //10000000
			case 0x30: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //00110000
			case 0x60: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //01100000
			case 0xc0: left_offset=0; right_offset=1;mid_offset=0;stop=0;break;	  //11000000
			
			//ƫ��״̬
			case 0x04: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000100
			case 0x02: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000010
			case 0x01: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000001
			case 0x03: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00001100
			case 0x06: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000110
			case 0x0c: left_offset=1; right_offset=0;mid_offset=0;stop=0;break;	  //00000011

			case 0xFF: stop=1; break;//ѭ��·�ں��� left_offset=1; right_offset=1;mid_offset=1;
			case 0x00: left_offset=0; right_offset=0;mid_offset=0;stop=0;break;//����
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
			if((left_offset==0)&(mid_offset==0)&(right_offset==0))	 //���磬����
				 {flag=3;stop=0;}
			else {flag=0;}											 //δ֪״̬��ֱ��
				 
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
