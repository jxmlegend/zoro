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

#define CAN_EN PORTBbits.RB1 //CAN使能
#define POWER_EN PORTBbits.RB0 //采集模块使能
#define min_AD 1100//电压值扩大1000后
#define max_AD 3500//电压值扩大1000后
#define    C_AD_CH0       0X00
#define    C_AD_CH1       0X04
#define	Size	10//串口传输的字节数

INT8U	recdata[Size]={0};	//接收数据寄存器
INT8U	num=0;//接收数据的数量

INT8U	SciReceiveFlag=0;	/* =1表示接收到新数据，＝0表示未接收到新数据 */
int	CAN_FLAG;						/* =1接收到CAN 数据,=0未接收到数据 */
INT8U Tim0_50ms = 0;//定时器50毫秒
INT8U ture=0;

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
** 函 数 名: unsigned int CheckSum(unsigned char *pbuf,unsigned char num)
** 功能描述: 校验串口接收到的数据是否正确
*************************************************************** */

INT8U CheckSum_char(INT8U num)
{  
	INT8U temp = 0;
	INT8U *pBUFF;
	pBUFF=&recdata[2];
		for(;num>0;num--)
		{
			temp+=*(pBUFF++);			
		}
   return (INT8U)temp;
}
/* ****************************************************************
** 函 数 名: INT8U Voltage_Tell(void)
** 功能描述: 校验电源电压是否在范围内
*************************************************************** */
INT8U Voltage_Tell(void)
{
	INT16U	battAD_val;
	INT16U	ignAD_val;
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
    INT8U  temp=0;
    INT8U	flag;
    flag=Voltage_Tell();
    
	if((Tim0_50ms==10)&&flag)
//	if(flag)
    {
	    Tim0_50ms=0;		/* 间隔一定时间发送CAN数据计时器清0 */
    	if(1==SciReceiveFlag)	/* 是否接收到通信数据 */
    	{	
		    SciReceiveFlag=0;
		    temp=CheckSum_char(6);
	        if((recdata[0]==0xaa)&&(recdata[1]==0xc0)&&(recdata[8]==temp)&&(recdata[9]==0xab))
//	 		if((recdata[0]==0xaa)&&(recdata[1]==0xc0)&&(recdata[9]==0xab))
	        {	
	           CAN_Transmit(recdata[3],recdata[2],recdata[5],recdata[4]);
	        }
	       RCSTA1bits.CREN=1; 
    	}  
// 	 	else CAN_Transmit(recdata[3],recdata[2],recdata[5],recdata[4]);//否则发送上次数据	
    }
    
}

void main()					
{
//	INT16U temp;
	initial();// 系统初始化
	InitCAN_Init();/* CAN设置初始化 */
	sciinitial();// 串行通讯初始化子程序
    Tim0Init();/* 定时器0设置初始化 */
    AD_Init();//ADC初始化
	
	RCONbits.IPEN=1;					// 使能中断高低优先级
	INTCON=INTCON|0xc0;		// 开总中断、开外围接口中断
	
	POWER_EN=1;
	CAN_EN =0;
	while(1)
	{
		PM25_Val();
//		temp=AD_GET(C_AD_CH1);	
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
        Tim0_50ms++;
        WriteTimer0( 15635 );//50ms中断一次
//	  	TMR0H=15635/256;		       
//      	TMR0L=15635%256;
        
    }
    if(PIR1bits.RC1IF==1)					// RS232接收中断
	{
		PIR1bits.RC1IF=0;					// 清接收中断标志
		recdata[num++]=RCREG1;			// 接收数据并存储
		if(recdata[0]==0xaa)
			ture=1;
		else 
			num--;
		if(ture)//表示收到的数据头正确
		{
			if(num>9)
			{
				num=0;
				SciReceiveFlag=1;		// 置接收到232数据标志
				ture=0;
				RCSTA1bits.CREN=0;				// 不允许串口接收数据
			}
		}	
	}
}
