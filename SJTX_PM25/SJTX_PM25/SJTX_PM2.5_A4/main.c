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

#define L_CAN_EN PORTBbits.RB1 //CANʹ��
#define H_POWER_EN PORTCbits.RC3 //�ɼ�ģ���Դʹ��
#define H_SLPN PORTCbits.RC4 //LINʹ��
#define H_WOEK_EN PORTCbits.RC2 //LINʹ��

#define min_AD 1100//��ѹֵ����1000��
#define max_AD 3500//��ѹֵ����1000��
#define    C_AD_CH0       0X00
#define    C_AD_CH1       0X04

INT8U	recdata[32]={0};	//�������ݼĴ���
INT8U	num=0;//�������ݵ�����
INT8U	SN=0;

INT8U	SciReceiveFlag=0;	/* =1��ʾ���յ������ݣ���0��ʾδ���յ������� */
int	CAN_FLAG;						/* =1���յ�CAN ����,=0δ���յ����� */
INT8U Tim0_500ms = 0;//��ʱ��500����
INT8U ture=0;

INT8U SCIDRL=0;//���ڶ�����
INT8U SCISR1_RDRF=0;//����2���ձ�־

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

//INT32U	battAD_val;//��ص�ѹ
//INT32U	ignAD_val;//��������ѹ

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
    TRISC = TRISC & 0xe3;
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
** �� �� ��: INT8U Voltage_Tell(void)
** ��������: У���Դ��ѹ�Ƿ��ڷ�Χ��
*************************************************************** */
INT8U Voltage_Tell(void)
{
	INT16U	battAD_val;//��ص�ѹ
	INT16U	ignAD_val;//��������ѹ
	
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
    INT16U  temp=0;
    INT16U  temp_pm25=0;
    INT16U  temp_pm10=0;
    INT8U	flag=0;
    
    flag=Voltage_Tell();
    
	if((Tim0_500ms==20)&&flag)
    {
	    Tim0_500ms=0;		/* ���һ��ʱ�䷢��CAN���ݼ�ʱ����0 */
    	if(1==SciReceiveFlag)	/* �Ƿ���յ�ͨ������ */
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
// 	 	else CAN_Transmit(recdata[6],recdata[7],recdata[8],recdata[9]);//�������ϴ�����
// 	 	else CAN_Transmit(recdata[0],recdata[1],recdata[6],recdata[7]);//�������ϴ�����	
    }
    
}


void main()					
{
	INT16U temp;
	initial();// ϵͳ��ʼ��	
	InitCAN_Init();/* CAN���ó�ʼ�� */
	sciinitial();// ����ͨѶ��ʼ���ӳ���
	sciinitial2();//���ڶ���ʼ��
	Tim0Init();/* ��ʱ��0���ó�ʼ�� */
    AD_Init();//ADC��ʼ��
	
	RCONbits.IPEN=1;					// ʹ���жϸߵ����ȼ�
	INTCON=INTCON|0xc0;		// �����жϡ�����Χ�ӿ��ж�
	//��Դʹ��
	H_POWER_EN=1;
	L_CAN_EN =0;
	//LINʹ����ͨб��ģʽ
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
        Tim0_500ms++;
        WriteTimer0( 15635 );//50ms�ж�һ��
//	  	TMR0H=15635/256;		       
//      	TMR0L=15635%256;
        
    }
    if(PIR1bits.RC1IF==1)					// RS232�����ж�
	{
		PIR1bits.RC1IF=0;					// ������жϱ�־
		recdata[num++]=RCREG1;			// �������ݲ��洢
		if(recdata[0]==0x42)
			ture=1;
		else 
			num--;
		if(ture)//��ʾ�յ�������ͷ��ȷ
		{
			if(num>31)
			{
				num=0;
				SciReceiveFlag=1;		// �ý��յ�232���ݱ�־
				ture=0;
				RCSTA1bits.CREN=0;				// �������ڽ�������
			}
		}	
	}
	if(PIR3bits.RC2IF==1)					// ����2�����ж�
	{
		SCIDRL=RCREG2;
        SCISR1_RDRF=1;
        PIR3bits.RC2IF=0;
	}
}
