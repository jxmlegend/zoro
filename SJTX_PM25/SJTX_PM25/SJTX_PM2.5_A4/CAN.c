/* ****************************************************************
** 功能描述: CAN自测试收发编程，CAN发送邮箱0以一定的时间间隔发送数据，接收到数据后,将接收数据加1作为下一次的发送数据
*************************************************************** */
#include "CAN.h"
					
#define	canRX_Adress 0x05fb /* CAN接收标识符 */
#define	canTX_Adress 0x05e2	/* CAN发送标识符 */
char	CAN_Adress_H,CAN_Adress_L;		/* CAN标识符高低字节 */
char	CAN_Adress_EH,CAN_Adress_EL;	/* CAN扩展标识符高低字节 */

void InitCAN_Init(void)
{
    CAN_Adress_H = canRX_Adress >>3;  // ID_CANRX_MSG_Pm25Out>>3;接收邮箱
    CAN_Adress_L = canRX_Adress <<5; //ID_CANRX_MSG_Pm25Out<<5;
    TRISB = (TRISB|0X80)&0Xbf;   
    CANCON = 0x80;                        //请求配置模式
    while(!(CANSTATbits.OPMODE ==0x04));  //确认CAN处于配置模式
    //CIOCON =  0X00;                       //CAN I/O口控制寄存器，设置PLL时钟

        ECANCON= 0x00;    //传统模式 ,将在剩余 4 个接收缓冲区时产生 FIFO 中断,接收过滤器 0、 1 和 2 以及 BRGCON2 和 BRGCON3                        
        CIOCON=0x21;      //使用振荡器作为 CAN 系统时钟源 ,隐性时 CANTX 引脚将驱动 VDD                                   
   
	    BRGCON1=0X00;	 //TQ = (2 x 1)/FOSC,00 = 同步跳转宽度时间 = 1 x TQ		  	          
        BRGCON2=0x98;    //0X90;	 可自由编程	相位缓冲段 1 时间 = 4 x TQ 传播时间 = 1 x TQ	 	        
        BRGCON3=0x41;    //0X42;使用 CAN 总线线路滤波器来唤醒 ,相位缓冲段 2 时间 = 2 x TQ
        		             
        BSEL0=0x00;         //缓冲区 5 至缓冲区 0 发送使能位,全部使能               
        RXB0CON = 0x20;    //0B00100000;  仅接收带有标准标识符的有效报文            
        RXB1CON = 0x00;    //根据 RXFnSIDL 寄存器中的 EXIDEN 位接收所有有效报文       

        RXB0SIDL=CAN_Adress_H;
	    RXB0SIDH=CAN_Adress_L & (0xE0);	
        RXB0DLC = 0X08;	         		      
        RXB0D0  = 0X00;	         		          
        RXB0D1  = 0X00;			      
        RXB0D2  = 0X00;
        RXB0D3  = 0X00;
        RXB0D4  = 0X00;
        RXB0D5  = 0X00;
        RXB0D6  = 0X00;
        RXB0D7  = 0X00;
       
       
       
        RXM0SIDH = 0xFF;   //0x18;   //0X98;                       
        RXM0SIDL = 0xE0;    //0X00;	
        
        RXFCON0 = 0x01;           
        //RXFCON1 = 0x00;     
        
        
        RXF0SIDH = 0x5fb >>3;  //0x67; 	  	          
        RXF0SIDL = (0x5fb <<5) & (0xE0);  //0xE0; 	
         
        CANCON=0x00; 

      
   
    while(CANSTATbits.OPMODE==0x00); 
       PIR5 = 0X00;					              
       PIE5 = PIE5 | 0X00;			 	         
       IPR5 = IPR5 | 0X00;                        
       PIR5bits.RXB0IF=0;
       PIE5bits.RXB0IE=0;   // 0
       IPR5bits.RXB0IP=1;
       RXB0CONbits.RXFUL=0;
       PIE5bits.RXB0IE=1;
//       CAN_Init_RxMsg();  
}


void CAN_Transmit(INT8U val1,INT8U val2,INT8U val3,INT8U val4)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //发送缓冲寄存器
     TXB0SIDH =  canTX_Adress>>3;                    //发送缓冲区标准标识符
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //发送缓冲区数据长度
     TXB0D0   =  val1;                    //发送缓冲区0数据字段字节0
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
     
//    TXB0D0=RXB0D0+1;  // 用接收数据加1来更新发送数据 */
    TXB0D1=val2;
    TXB0D2=val3;
    TXB0D3=val4;
    TXB0D4=0x00;
    TXB0D5=0x00;
    TXB0D6=0x00;
    TXB0D7=0x00; 
//     TXB0CON  =  TXB0CON | 0x08;          //请求发送报文
	TXB0CONbits.TXREQ = 1;     // 启动发送。
	while(TXB0CONbits.TXREQ);   // 缓冲器有没有报文正在发送，有，等待。
}

void CAN_Transmit2(unsigned int Pm10_Value,unsigned int Pm25_Value,unsigned char num)
//void CAN_Transmit2(INT8U val1,INT8U val2,INT8U val3,INT8U val4,INT8U val5)//PM2.5(1-2) SN号(3) PM10(4-5)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //发送缓冲寄存器
     TXB0SIDH =  canTX_Adress>>3;                    //发送缓冲区标准标识符
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //发送缓冲区数据长度
//     TXB0D0   =  (val1<<6)|(val2>>2);                    //发送缓冲区0数据字段字节0
//    
//    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
//     
////    TXB0D0=RXB0D0+1;  // 用接收数据加1来更新发送数据 */
//    TXB0D1=(val2<<6)|((val3<<1)&0x3e)|((val4>>1)&0x01);
//    TXB0D2=val4<<7|val5>>1;
//    TXB0D3=val5<<7;
	 TXB0D0   =  0xFF;                    //发送缓冲区0数据字段字节0
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST

     TXB0D1   =  0x3E&num<<1;
     TXB0D1   =  TXB0D1|(0xc0&Pm25_Value<<6);
     TXB0D0   =  0xff&Pm25_Value>>2;
     TXB0D3   =  0x80&Pm10_Value<<7;
//     TXB0D3   |=  0xff&Degree<<3;      //add the Level display 2018.08.12
     TXB0D2   =  0xff&Pm10_Value>>1;
     TXB0D1   |=  0x01&Pm10_Value>>9;
    TXB0D4=0x00;
    TXB0D5=0x00;
    TXB0D6=0x00;
    TXB0D7=0x00; 
//     TXB0CON  =  TXB0CON | 0x08;          //请求发送报文
	TXB0CONbits.TXREQ = 1;     // 启动发送。
	while(TXB0CONbits.TXREQ);   // 缓冲器有没有报文正在发送，有，等待。
}
void CAN_Transmit3(INT8U val1,INT8U val2,INT8U val3,INT8U val4,INT8U val5)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //发送缓冲寄存器
     TXB0SIDH =  canTX_Adress>>3;                    //发送缓冲区标准标识符
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //发送缓冲区数据长度
     TXB0D0   =  val1<<6 &0xc0; //发送缓冲区0数据字段字节0
     TXB0D0  |=  val2>>2 &0x3f;                
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
     
//    TXB0D0=RXB0D0+1;  // 用接收数据加1来更新发送数据 */
    TXB0D1 = val2<<6 &0xc0;
	TXB0D1|= val5 <<1 &0x3e;
	TXB0D1|= val3 >>1 &0x01;
	
    TXB0D2 = val3 <<7&0x80;
    TXB0D2|= val4>>1 & 0x7f;
    
    TXB0D3 = val4<<7 &0x80;
    TXB0D4=0x00;
    TXB0D5=0x00;
    TXB0D6=0x00;
    TXB0D7=0x00; 
//     TXB0CON  =  TXB0CON | 0x08;          //请求发送报文
	TXB0CONbits.TXREQ = 1;     // 启动发送。
	while(TXB0CONbits.TXREQ);   // 缓冲器有没有报文正在发送，有，等待。
}