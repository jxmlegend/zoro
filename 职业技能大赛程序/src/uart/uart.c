#include "uart.h"
#include "delay.h"
#include "RC.h"



void UartInit(void)		//9600bps@11.0592MHz
{
  PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xDC;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
  TI=1;
}

void uart_senddata(uchar ch)//�򴮿ڷ�������
{

   SBUF=ch;
   while(TI==0); //�ȴ��������
	TI=0;
}

void uart_BYTE (uchar *p,uchar length)
{
	  while(length!=0)
	  {
	  	uart_senddata(*p);
		p++;
		length--;
	  }
}

 
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
