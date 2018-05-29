#ifndef _UART_H_
#define _UART_H_

#include "commom.h"

#define   Controlled_Car_CMD					0x11		//被控小车
#define 	Dot_Matrix_CMD							0x12 		//被控点阵语音播报显示屏
#define   Controlled_Rotating_LED			0x13		//旋转LED
#define   Controlled_Fan							0x14    //风扇
#define   Controlled_Warning_Light		0x15		//警示灯
#define   Controlled_Gateway					0x16 		//闸道

void UartInit(void);	//19200bps@11.0592MHz
void uart_senddata(uchar ch);//向串口发送数据
void uart_BYTE (uchar *p,uchar length);
uchar CheckSum(uchar *pbuf,uchar num);
void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl);

#endif