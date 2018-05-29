#ifndef __ZigBee_H
#define __ZigBee_H	 
#include "sys.h"

#define   Controlled_Car_CMD					0x11		//����С��
#define 	Dot_Matrix_CMD							0x12 		//���ص�������������ʾ��
#define   Controlled_Rotating_LED			0x13		//��תLED
#define   Controlled_Fan							0x14    //����
#define   Controlled_Warning_Light		0x15		//��ʾ��
#define   Controlled_Gateway					0x16 		//բ��

void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl);
uchar CheckSum(uchar *pbuf,uchar num);



		 				    
#endif
