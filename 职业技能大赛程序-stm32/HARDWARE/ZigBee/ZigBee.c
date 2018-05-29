#include "ZigBee.h"
#include "usart.h"	


/*
 函数功能：  校验和
 返回：      数组和取反的值
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
   参数：
					address      与被控装置连接的zigbee模块的16位地址；
						cmd：        控制不同装置的命令 
				  SeqNum：			序号     限定在1到6之间
				  Ctrl：			  工位号		限定在1到60之间
												控制被控小车的动作
*/
void zigbee_transmint_data(uint address,uchar cmd,uchar SeqNum, uchar Ctrl)
{
		u8 tmp_buf[9];
		tmp_buf[0]=0xcc;							   //帧头
	  tmp_buf[1]=0x33;
		tmp_buf[2]=address>>8;			   	//与被控装置连接的zigbee模块的16位地址；
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
		tmp_buf[8]=CheckSum((tmp_buf+4),4);//注意此处：只求和并取反  tmp_buf[4]到temp_buf[7]之间的数据
//		U1_printf(tmp_buf);
		uart_BYTE(tmp_buf,9);
		
}


