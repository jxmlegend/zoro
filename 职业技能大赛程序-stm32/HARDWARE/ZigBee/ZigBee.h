#ifndef __ZigBee_H
#define __ZigBee_H	 
#include "sys.h"

#define   Controlled_Car_CMD					0x11		//被控小车
#define 	Dot_Matrix_CMD							0x12 		//被控点阵语音播报显示屏
#define   Controlled_Rotating_LED			0x13		//旋转LED
#define   Controlled_Fan							0x14    //风扇
#define   Controlled_Warning_Light		0x15		//警示灯
#define   Controlled_Gateway					0x16 		//闸道

void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl);
uchar CheckSum(uchar *pbuf,uchar num);



		 				    
#endif
