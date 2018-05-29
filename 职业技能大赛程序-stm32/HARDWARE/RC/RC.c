#include "RC.h"
#include "24l01.h"
#include "led.h"
#include "delay.h"
#include "ZigBee.h"

/**************************************************************************
函数功能：无线模块自检
入口参数：无
返回  值：无
**************************************************************************/
void NRF24L01_FindMyself(void)
{
		while(NRF24L01_Check())//检测不到24L01
	{
		LED1=!LED1;//DS0闪烁
		delay_ms(500); 
	}
}

void NRF24L01_transmit(uchar cmd)
{
    u8 tmp_buf[5];
	  switch(cmd)
		{
		  case Light_1:  
												tmp_buf[0]=0xaa;//1等
												tmp_buf[1]=0xbb;
												tmp_buf[2]=0x49;//1等
												tmp_buf[3]=0x68; 
			                 break;
			case Light_2:			tmp_buf[0]=0xaa;//1等
												tmp_buf[1]=0xbb;
												tmp_buf[2]=0x50;//2等
												tmp_buf[3]=0x68;
			                 break;			
      case Light_3:		 tmp_buf[0]=0xaa;//1等
											 tmp_buf[1]=0xbb;
												tmp_buf[2]=0x51;//3等
											 tmp_buf[3]=0x68;
			                 break;			
      case Light_4:		 tmp_buf[0]=0xaa;//1等
											 tmp_buf[1]=0xbb;
												tmp_buf[2]=0x52;//4等
											 tmp_buf[3]=0x68;		    
			                 break;			
		  case ZhaDa:			 tmp_buf[0]=0xaa;//1等
											 tmp_buf[1]=0xbb;
											 tmp_buf[2]=0x84;//抬起
											 tmp_buf[3]=0x81; 
			                 break;			
			case Alert_led:  tmp_buf[0]=0xaa;//1等
											 tmp_buf[1]=0xbb;
											 tmp_buf[2]=0x66;//警灯
											 tmp_buf[3]=0x68;
			                 break;	
			case Light_off:  tmp_buf[0]=0xaa;//1等
											 tmp_buf[1]=0xbb;
											 tmp_buf[2]=0x53;//警灯
											 tmp_buf[3]=0x68;
			                 break;											
			default:         tmp_buf[0]=0x00;
											 tmp_buf[1]=0x00;
			                 break;		 			
		}
	 tmp_buf[4]=CheckSum(tmp_buf,4);
		 if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
	         LED0=!LED0;			
}






