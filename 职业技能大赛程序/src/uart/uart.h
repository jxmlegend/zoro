#ifndef _UART_H_
#define _UART_H_

#include "commom.h"

#define   Controlled_Car_CMD					0x11		//����С��
#define 	Dot_Matrix_CMD							0x12 		//���ص�������������ʾ��
#define   Controlled_Rotating_LED			0x13		//��תLED
#define   Controlled_Fan							0x14    //����
#define   Controlled_Warning_Light		0x15		//��ʾ��
#define   Controlled_Gateway					0x16 		//բ��

void UartInit(void);	//19200bps@11.0592MHz
void uart_senddata(uchar ch);//�򴮿ڷ�������
void uart_BYTE (uchar *p,uchar length);
uchar CheckSum(uchar *pbuf,uchar num);
void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl);

#endif