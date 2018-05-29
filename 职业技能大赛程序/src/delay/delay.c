#include "delay.h"


void delay(uint z)			  //��ʱ�Ӻ���
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=114;y>0;y--);
}

/********************************************************************
* ���� : Delay_1ms()
* ���� : ��ʱ�ӳ�����ʱʱ��Ϊ 1ms * x
* ���� : x (��ʱһ����ĸ���)
* ��� : ��
***********************************************************************/
void Delay_1ms(uint i)//1ms��ʱ
{
	uint x,j;
	for(j=0;j<i;j++)
	for(x=0;x<=148;x++); 
}

void delay_ms(uint z)			  //��ʱ�Ӻ���
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=1200;y>0;y--);
}

  void delay_us(uint z)			  //��ʱ�Ӻ���
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=114;y>0;y--);
}

void Delay180ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 8;
	j = 145;
	k = 99;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay300ms()		//@11.0592MHz
{
unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 11;
	j = 130;
	k = 111;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	}  while (--i);
}
void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}