/* ****************************************************************
** ��������: 
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

#define CAN_EN PORTBbits.RB1 //CANʹ��
#define POWER_EN PORTBbits.RB0 //�ɼ�ģ��ʹ��
#define min_AD 1100//��ѹֵ����1000��
#define max_AD 3500//��ѹֵ����1000��
#define    C_AD_CH0       0X00
#define    C_AD_CH1       0X04
#define	Size	10//���ڴ�����ֽ���

INT8U	recdata[Size]={0};	//�������ݼĴ���
INT8U	num=0;//�������ݵ�����

INT8U	SciReceiveFlag=0;	/* =1��ʾ���յ������ݣ���0��ʾδ���յ������� */
int	CAN_FLAG;						/* =1���յ�CAN ����,=0δ���յ����� */
INT8U Tim0_50ms = 0;//��ʱ��50����
INT8U ture=0;

void usart_isr();
void isr_high();
/* ****************************************************************
** �� �� ��: initial()
** ��������: ϵͳ��ʼ���ӳ��򣬷��ڳ����ײ�
*************************************************************** */
void	initial(void)
{
	INTCON=0x00;			/* bit7-bit0:�����ж� */
//	ADCON1=0X07;			/* ����������������� */
	PIE1=0;					/* PIE1 ���жϲ�ʹ�� */			
	PIE2=0;					/* PIE2 ���жϲ�ʹ�� */	
	PIE3=0;					/* PIE3 ���жϲ�ʹ�� */	
    PIE4=0;					/* PIE4 ���жϲ�ʹ�� */	
    PIE5=0;					/* PIE5 ���жϲ�ʹ�� */	
    //LED PB0�˿ڳ�ʼ��
    TRISB = TRISB & 0xfc;
//	TRISB = 0x00;
}
/* ****************************************************************
** �� �� ��: CAN_Time_Init()
** ��������: CANʱ�ӳ�ʼ��
*************************************************************** */
void TimeOS_Init(void)
{ 
    OSCTUNEbits.PLLEN = 1;
    OSCCONbits.IRCF =   7;      //add 16M
}

/* ****************************************************************
** �� �� ��: unsigned int CheckSum(unsigned char *pbuf,unsigned char num)
** ��������: У�鴮�ڽ��յ��������Ƿ���ȷ
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
** �� �� ��: unsigned int CheckSum(unsigned char *pbuf,unsigned char num)
** ��������: У�鴮�ڽ��յ��������Ƿ���ȷ
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
** �� �� ��: INT8U Voltage_Tell(void)
** ��������: У���Դ��ѹ�Ƿ��ڷ�Χ��
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
** �� �� ��: PM25_Val()
** ��������: �ж�PM2.5�������Ƿ������յ������յ������ݴ���
*************************************************************** */
void PM25_Val(void)
{ 
    INT8U  temp=0;
    INT8U	flag;
    flag=Voltage_Tell();
    
	if((Tim0_50ms==10)&&flag)
//	if(flag)
    {
	    Tim0_50ms=0;		/* ���һ��ʱ�䷢��CAN���ݼ�ʱ����0 */
    	if(1==SciReceiveFlag)	/* �Ƿ���յ�ͨ������ */
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
// 	 	else CAN_Transmit(recdata[3],recdata[2],recdata[5],recdata[4]);//�������ϴ�����	
    }
    
}

void main()					
{
//	INT16U temp;
	initial();// ϵͳ��ʼ��
	InitCAN_Init();/* CAN���ó�ʼ�� */
	sciinitial();// ����ͨѶ��ʼ���ӳ���
    Tim0Init();/* ��ʱ��0���ó�ʼ�� */
    AD_Init();//ADC��ʼ��
	
	RCONbits.IPEN=1;					// ʹ���жϸߵ����ȼ�
	INTCON=INTCON|0xc0;		// �����жϡ�����Χ�ӿ��ж�
	
	POWER_EN=1;
	CAN_EN =0;
	while(1)
	{
		PM25_Val();
//		temp=AD_GET(C_AD_CH1);	
	}
}

/* ****************************************************************
** �� �� ��: interrupt	isr_high_direct	isr_high()
** ��������: �����ȼ��ж��ӳ���CAN�жϽ��ա�TIM0�ж�
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
    if(PIR5bits.RXB0IF==1)				/* RXB0��������0�����ж� */
	{
   		CAN_FLAG=1;			/* �ý��յ� CAN ���� */
		PIR5bits.RXB0IF=0;				/* ������жϱ�־ */
		RXB0CONbits.RXFUL=0;				/* bit7��0�򿪽��ջ�������������Ϣ */
	}
	 if(INTCONbits.T0IF==1)
    {
        INTCONbits.T0IF=0;
        Tim0_50ms++;
        WriteTimer0( 15635 );//50ms�ж�һ��
//	  	TMR0H=15635/256;		       
//      	TMR0L=15635%256;
        
    }
    if(PIR1bits.RC1IF==1)					// RS232�����ж�
	{
		PIR1bits.RC1IF=0;					// ������жϱ�־
		recdata[num++]=RCREG1;			// �������ݲ��洢
		if(recdata[0]==0xaa)
			ture=1;
		else 
			num--;
		if(ture)//��ʾ�յ�������ͷ��ȷ
		{
			if(num>9)
			{
				num=0;
				SciReceiveFlag=1;		// �ý��յ�232���ݱ�־
				ture=0;
				RCSTA1bits.CREN=0;				// �������ڽ�������
			}
		}	
	}
}
