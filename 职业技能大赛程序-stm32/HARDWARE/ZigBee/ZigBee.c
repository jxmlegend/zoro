#include "ZigBee.h"
#include "usart.h"	


/*
 �������ܣ�  У���
 ���أ�      �����ȡ����ֵ
*/

uchar CheckSum(uchar *pbuf,uchar num)
{
	 uchar temp = 0;
		for(;num>0;num--)
		{
			 temp+=*pbuf++;			
		}
    temp=~temp;
   return (uchar)temp;
}

/*
   ������
					address      �뱻��װ�����ӵ�zigbeeģ���16λ��ַ��
						cmd��        ���Ʋ�ͬװ�õ����� 
				  SeqNum��			���     �޶���1��6֮��
				  Ctrl��			  ��λ��		�޶���1��60֮��
												���Ʊ���С���Ķ���
*/
void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl)
{
		u8 tmp_buf[9];
		tmp_buf[0]=0xcc;							   //֡ͷ
	  tmp_buf[1]=0x33;
		tmp_buf[2]=address>>8;			   	//�뱻��װ�����ӵ�zigbeeģ���16λ��ַ��
	  tmp_buf[3]=address&0x00ff;
		tmp_buf[4]=0x55;
	  tmp_buf[5]=cmd;
		switch (tmp_buf[5])	
		{
			case Controlled_Car_CMD:		
				tmp_buf[6]=SeqNum;
				tmp_buf[7]=Ctrl;
				break;
			case Dot_Matrix_CMD:
				if(SeqNum<1) 					SeqNum=1;
				else if(SeqNum>6)			SeqNum=6;
				tmp_buf[6]=SeqNum;
				if(Ctrl<1) 				  Ctrl=1;
				else if(Ctrl>60)		Ctrl=60;
				tmp_buf[7]=Ctrl;
				break;
			case Controlled_Rotating_LED:
				tmp_buf[6]=0x01;
				tmp_buf[7]=0xFE;
				break;										
			case Controlled_Warning_Light:
				tmp_buf[6]=0x01;
				tmp_buf[7]=0xFE;
				break;	
			case Controlled_Gateway :
				tmp_buf[6]=0x01;
				tmp_buf[7]=0xFE;
				break;
		}
		tmp_buf[8]=CheckSum((tmp_buf+4),4);//ע��˴���ֻ��Ͳ�ȡ��  tmp_buf[4]��temp_buf[7]֮�������
//		U1_printf(tmp_buf);
		uart_BYTE(tmp_buf,9);
		
}


