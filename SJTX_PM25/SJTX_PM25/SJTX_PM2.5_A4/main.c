/* ****************************************************************
** 功能描述: 
*************************************************************** */
#include	<p18cxxx.h>
#include	<delays.h>
#include	<timers.h>
#include	"time.h"
#include	"CAN.h"
#include	"usart.h"
#include	"AD.h"
#include    "Lin.h"

/**********************************************************************************************************************/
#pragma config  IESO   = ON ,      FCMEN   = ON,       PLLCFG    = OFF,        FOSC   = HS1      /*  CONFIG1H*/
#pragma config  XINST  = OFF,      SOSCSEL = DIG,      INTOSCSEL = LOW,        RETEN  = OFF       /*  CONFIG1L*/

#pragma config  BORPWR = MEDIUM,    BORV    = 0,       PWRTEN = OFF       /*  CONFIG2L*/ 
#pragma config  WDTEN  = OFF,      BOREN     = SBORDIS,    WDTPS   = 2048                    /*  CONFIG2H*/ 

#pragma config  MCLRE  = ON,       CANMX   = PORTB                                           /*  CONFIG3H*/  
#pragma config  STVREN = OFF,      BBSIZ   = BB2K                                            /*  CONFIG4L*/
 
#pragma config  CP0    = OFF,      CP1     = OFF,   CP2      = OFF,       CP3    = OFF      /*  CONFIG5L*/
#pragma config  CPB    = OFF,      CPD     = OFF                                            /*  CONFIG5H*/

#pragma config  WRT0   = OFF,      WRT1    = OFF,   WRT2     = OFF,       WRT3   = OFF      /*  CONFIG6L*/
#pragma config  WRTB   = OFF,      WRTC    = OFF,   WRTD     = OFF                          /*  CONFIG6H*/

#pragma config  EBTR0  = OFF,      EBTR1   =  OFF,  EBTR2    = OFF,       EBTR3  = OFF      /*  CONFIG7L*/
#pragma config  EBTRB  = OFF  

#define L_CAN_EN PORTBbits.RB1 //CAN使能
#define H_POWER_EN PORTCbits.RC3 //采集模块电源使能
#define H_SLPN PORTCbits.RC4 //LIN使能
#define H_WOEK_EN PORTCbits.RC2 //LIN使能

#define min_AD 1100//电压值扩大1000后
#define max_AD 3500//电压值扩大1000后
#define    C_AD_CH0       0X00
#define    C_AD_CH1       0X04

INT8U	recdata[32]={0};	//接收数据寄存器
INT8U	num=0;//接收数据的数量
INT8U	SN=0;

INT8U	SciReceiveFlag=0;	/* =1表示接收到新数据，＝0表示未接收到新数据 */
int	CAN_FLAG;						/* =1接收到CAN 数据,=0未接收到数据 */
INT8U Tim0_500ms = 0;//定时器500毫秒
INT8U ture=0;

INT8U SCIDRL=0;//串口二数据
INT8U SCISR1_RDRF=0;//串口2接收标志

extern LIN_Msg  _SlaveSendMsg;
extern LIN_Frame SlaveFrame_Receive;
// INT8U  buff[4];
// INT8U  Send_Enable_Flag=0;
//  INT8U  send[6];
//   INT8U  Rec_Data[3];
//volatile INT8U RC_Falg;
//volatile INT8U   Rec_Start_Flag;
//volatile INT8U   Rec_Send_Flag;
//volatile INT8U  Pot;

//INT32U	battAD_val;//电池电压
//INT32U	ignAD_val;//发动机电压

void usart_isr();
void isr_high();
/* ****************************************************************
** 函 数 名: initial()
** 功能描述: 系统初始化子程序，放在程序首部
*************************************************************** */
void	initial(void)
{
	INTCON=0x00;			/* bit7-bit0:关总中断 */
//	ADCON1=0X07;			/* 设置数字输入输出口 */
	PIE1=0;					/* PIE1 的中断不使能 */			
	PIE2=0;					/* PIE2 的中断不使能 */	
	PIE3=0;					/* PIE3 的中断不使能 */	
    PIE4=0;					/* PIE4 的中断不使能 */	
    PIE5=0;					/* PIE5 的中断不使能 */	
    //LED PB0端口初始化
    TRISB = TRISB & 0xfc;
    TRISC = TRISC & 0xe3;
//	TRISB = 0x00;
}
/* ****************************************************************
** 函 数 名: CAN_Time_Init()
** 功能描述: CAN时钟初始化
*************************************************************** */
void TimeOS_Init(void)
{ 
    OSCTUNEbits.PLLEN = 1;
    OSCCONbits.IRCF =   7;      //add 16M
}

/* ****************************************************************
** 函 数 名: unsigned int CheckSum(unsigned char *pbuf,unsigned char num)
** 功能描述: 校验串口接收到的数据是否正确
*************************************************************** */

INT16U CheckSum_int(INT8U *pbuf,INT8U num)
{  
	INT16U temp = 0;
		for(;num>0;num--)
		{
			temp+=*pbuf++;			
		}
   return (INT16U)temp;
}
/* ****************************************************************
** 函 数 名: INT8U Voltage_Tell(void)
** 功能描述: 校验电源电压是否在范围内
*************************************************************** */
INT8U Voltage_Tell(void)
{
	INT16U	battAD_val;//电池电压
	INT16U	ignAD_val;//发动机电压
	
	battAD_val=AD_GET(C_AD_CH1);
	ignAD_val=AD_GET(C_AD_CH0);
	
	if((battAD_val>min_AD)&&(ignAD_val>min_AD))
	{
		if((battAD_val<max_AD)&&(ignAD_val<max_AD))
		{
			return 1;
		}
	}
	else 
		return 0;
}
/* ****************************************************************
** 函 数 名: PM25_Val()
** 功能描述: 判断PM2.5的数据是否完整收到并将收到的数据储存
*************************************************************** */
void PM25_Val(void)
{ 
    INT16U  temp=0;
    INT16U  temp_pm25=0;
    INT16U  temp_pm10=0;
    INT8U	flag=0;
    
    flag=Voltage_Tell();
    
	if((Tim0_500ms==20)&&flag)
    {
	    Tim0_500ms=0;		/* 间隔一定时间发送CAN数据计时器清0 */
    	if(1==SciReceiveFlag)	/* 是否接收到通信数据 */
    	{	
		    SciReceiveFlag=0;
		    temp=CheckSum_int(recdata,29);
	
	        if(recdata[0]==0x42&&recdata[1]==0x4d&&recdata[30]==(temp/256)&&recdata[31]==(temp%256))
	 
	        {
		       SN++;
		       if(SN>29) SN=0;
		       
		       temp_pm25=recdata[6]*256+recdata[7];
		       temp_pm10=recdata[8]*256+recdata[9];
//	           CAN_Transmit(recdata[6],recdata[7],recdata[8],recdata[9]);
			   CAN_Transmit2(temp_pm10,temp_pm25,SN);
//	           CAN_Transmit(recdata[0],recdata[1],recdata[6],recdata[7]);
//			   CAN_Transmit3(recdata[6],recdata[7],recdata[8],recdata[9],SN);
			   
	        }
	       RCSTA1bits.CREN=1; 
    	}  
// 	 	else CAN_Transmit(recdata[6],recdata[7],recdata[8],recdata[9]);//否则发送上次数据
// 	 	else CAN_Transmit(recdata[0],recdata[1],recdata[6],recdata[7]);//否则发送上次数据	
    }
    
}


void main()					
{
	INT16U temp;
	initial();// 系统初始化	
	InitCAN_Init();/* CAN设置初始化 */
	sciinitial();// 串行通讯初始化子程序
	sciinitial2();//串口二初始化
	Tim0Init();/* 定时器0设置初始化 */
    AD_Init();//ADC初始化
	
	RCONbits.IPEN=1;					// 使能中断高低优先级
	INTCON=INTCON|0xc0;		// 开总中断、开外围接口中断
	//电源使能
	H_POWER_EN=1;
	L_CAN_EN =0;
	//LIN使能普通斜率模式
	H_SLPN=1;
	H_WOEK_EN=1;
	while(1)
	{
 
        if(Tim0_500ms==20)
        {
            Tim0_500ms=0;
            LINMaster_Send_Msg(&_SlaveSendMsg,0);
        }
    
//		PM25_Val();
//		temp=AD_GET(C_AD_CH1);	
//		CAN_Transmit2(0x01,56,0x03);
	}
}

/* ****************************************************************
** 函 数 名: interrupt	isr_high_direct	isr_high()
** 功能描述: 高优先级中断子程序：CAN中断接收、TIM0中断
*************************************************************** */
#pragma  code isrhighcode = 0x0008        /*the address is 0x008h*/ 
void isr_high_direct(void)   
{                                        /*go to the interupt */  
  _asm                                  /*begin in-line assembly */
  goto isr_high                        /*go to isr_high function*/
  _endasm                             /*end in-line assembly*/
} 
#pragma code                                   
#pragma  interrupt  isr_high         /*this is a interupt function*/ 

void isr_high(void)
{
    if(PIR5bits.RXB0IF==1)				/* RXB0接收邮箱0接收中断 */
	{
   		CAN_FLAG=1;			/* 置接收到 CAN 数据 */
		PIR5bits.RXB0IF=0;				/* 清接收中断标志 */
		RXB0CONbits.RXFUL=0;				/* bit7＝0打开接收缓冲器接收新信息 */
	}
	 if(INTCONbits.T0IF==1)
    {
        INTCONbits.T0IF=0;
        Tim0_500ms++;
        WriteTimer0( 15635 );//50ms中断一次
//	  	TMR0H=15635/256;		       
//      	TMR0L=15635%256;
        
    }
    if(PIR1bits.RC1IF==1)					// RS232接收中断
	{
		PIR1bits.RC1IF=0;					// 清接收中断标志
		recdata[num++]=RCREG1;			// 接收数据并存储
		if(recdata[0]==0x42)
			ture=1;
		else 
			num--;
		if(ture)//表示收到的数据头正确
		{
			if(num>31)
			{
				num=0;
				SciReceiveFlag=1;		// 置接收到232数据标志
				ture=0;
				RCSTA1bits.CREN=0;				// 不允许串口接收数据
			}
		}	
	}
	if(PIR3bits.RC2IF==1)					// 串口2接收中断
	{
		SCIDRL=RCREG2;
        SCISR1_RDRF=1;
        PIR3bits.RC2IF=0;
	}
}
