#ifndef _DELAY_H_
#define _DELAY_H_


#include "commom.h"


void delay(uint z);//@12.000MHz
void Delay_1ms(uint i);//1ms��ʱ

void delay_ms(uint z);			  //��ʱ�Ӻ���
  void delay_us(uint z);			  //��ʱ�Ӻ���
	void Delay180ms();		//@11.0592MHz
	void Delay300ms();	//@11.0592MHz
	void Delay500ms();	//@11.0592MHz
#endif
