#include "RC.h"
#include "NRF24L01.H"
#include "delay.h"
#include "uart.h"

uchar CheckSum(uchar *pbuf,uchar num);
void NRF24L01_transmit(uchar cmd)
{
   u8 tmp_buf[5];
	 tmp_buf[0]=0xaa;
	 tmp_buf[1]=0xbb;
	  switch(cmd)
		{
		  case Light_1:  		
				               tmp_buf[2]=0x49;//1��
											 tmp_buf[3]=0x68; 
			                 break;
			case Light_2:		 tmp_buf[2]=0x50;//2��
											 tmp_buf[3]=0x68;
			                 break;			
      case Light_3:		 tmp_buf[2]=0x51;//3��
											 tmp_buf[3]=0x68;
			                 break;			
      case Light_4:		 tmp_buf[2]=0x52;//4��
											 tmp_buf[3]=0x68;		    
			                 break;			
		  case ZhaDao:		 tmp_buf[2]=0x84;//̧��
											 tmp_buf[3]=0x81; 
			                 break;			
			case Alert_led:  tmp_buf[2]=0x66;//����
											 tmp_buf[3]=0x68;
			                 break;	
			case Light_off:  tmp_buf[2]=0x53;//����
											 tmp_buf[3]=0x68;
			                 break;
			default:         tmp_buf[2]=0x00;
											 tmp_buf[3]=0x00;
			                 break;		 												 
		}
	  	tmp_buf[4]=CheckSum(tmp_buf,4);	  
		 if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
	    {   
			   uart_senddata(0x66) ;	 
		 } 
} 

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


/**************************************************************************
�������ܣ�����ģ���Լ�
��ڲ�������
����  ֵ����
**************************************************************************/
void NRF24L01_FindMyself(void)
{
		while(NRF24L01_Check())//��ⲻ��24L01
	{
		   BEEP=!BEEP;//DS0��˸
		   Delay300ms();
	}
	 BEEP=1;
}

