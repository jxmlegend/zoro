#include "delay.h"


void delay(uint z)			  //延时子函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=114;y>0;y--);
}

/********************************************************************
* 名称 : Delay_1ms()
* 功能 : 延时子程序，延时时间为 1ms * x
* 输入 : x (延时一毫秒的个数)
* 输出 : 无
***********************************************************************/
void Delay_1ms(uint i)//1ms延时
{
	uint x,j;
	for(j=0;j<i;j++)
	for(x=0;x<=148;x++); 
}

void delay_ms(uint z)			  //延时子函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=1200;y>0;y--);
}

  void delay_us(uint z)			  //延时子函数
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