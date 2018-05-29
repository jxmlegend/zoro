#include  "p18f25k80.h"
#include  "input.h"
//#include  "can.h"

/**********************************************************************************************************************/
#pragma config  IESO   = ON ,      FCMEN   = ON,       PLLCFG    = OFF,        FOSC   = HS1      /*  CONFIG1H*/
#pragma config  XINST  = OFF,      SOSCSEL = DIG,      INTOSCSEL = LOW,        RETEN  = OFF       /*  CONFIG1L*/

#pragma config  BORPWR = MEDIUM,    BORV    = 0,       PWRTEN = OFF       /*  CONFIG2L*/ 
#pragma config  WDTEN  = OFF,      BOREN     = SBORDIS,    WDTPS   = 2048                    /*  CONFIG2H*/ 

#pragma config  MCLRE  = ON,       CANMX   = PORTC                                           /*  CONFIG3H*/  
#pragma config  STVREN = OFF,      BBSIZ   = BB2K                                            /*  CONFIG4L*/
 
#pragma config  CP0    = OFF,      CP1     = OFF,   CP2      = OFF,       CP3    = OFF      /*  CONFIG5L*/
#pragma config  CPB    = OFF,      CPD     = OFF                                            /*  CONFIG5H*/

#pragma config  WRT0   = OFF,      WRT1    = OFF,   WRT2     = OFF,       WRT3   = OFF      /*  CONFIG6L*/
#pragma config  WRTB   = OFF,      WRTC    = OFF,   WRTD     = OFF                          /*  CONFIG6H*/

#pragma config  EBTR0  = OFF,      EBTR1   =  OFF,  EBTR2    = OFF,       EBTR3  = OFF      /*  CONFIG7L*/
#pragma config  EBTRB  = OFF  


#define   POWER    PORTBbits.RB1  
#define   CAN_EN   PORTCbits.RC4    //  RC3 //for can_enable controlling

#define  INT8U     unsigned char
#define  INT16U    unsigned int
#define  INT32U    unsigned long

#define  C_TIMER_1MS     0xF84c
#define  CAN_ID          0x05Fb

#define  AVG_LEN         60


unsigned int Timer_Count_Second;
unsigned int LPWM_Timer_Count;
unsigned int LPWM_SUM_timer;
unsigned char LPWM_Timer_Flag;
unsigned int LPWM_Duty_Buffer[60]={0};
unsigned char Buffer_Flag;
unsigned char Buffer_Flag_Old=11;    // org:31
volatile unsigned int PM25_Value;
unsigned int LPWM_Buffer[100] = {0};
unsigned int PM25_Display;            //added code:20161208
unsigned int Display_Counter=0;         //added code:20161208
unsigned int LPWM_Duty;
unsigned int LPWM_Display;               //added code:20161208
unsigned char CAN_Timer;
unsigned char CAN_send_flag=0;
unsigned char num,Level,Level_old;       // review 2018.08.12
long int LPWM_SUM;
long int LPWM_Total;
unsigned int LPWM_AVG = 0;
unsigned int PM25_Bias,PM25_Inc;   //for controling the increase/decrease speed,date:2017.07.27
unsigned int LPWM_Duty_old;
unsigned int PM25_Value_old;
unsigned int Display_Counter_Old=0;
unsigned char CANR_FLAG;
unsigned char CNT_Duty =0;
unsigned char CANR_OUT_OK;

extern   INT16U     CANR_Pm25OutValue;
extern   INT8U      CANR_Pm25OutSn;
extern INT8U CANR_SIDH;
extern INT8U CANR_SIDL;

extern typedef  struct
{
 INT8U     Status;

 INT8U     InitCount;
 INT8U     InitTimeoutCnt;

 INT8U     BusOff;
 INT16U    BusOffCount;
 
 
 INT8U     Pm25OutStatus;   //for Pm25Out pannel test tooling;
  
   
 
 INT16U    Pm25OutTimeoutCnt;  //for Pm25Out Pannel testt tooling;

 INT16U    ACSendTimeroutcnt1;  
 INT16U    ACSendTimeroutcnt2;     
          
}CAN_DRIVER;
//extern struct IGN_Feedback;
//extern struct BATT_Feedback;
extern SUPPLY_VOLTAGE_CONTROL   IGN_Feedback;

extern SUPPLY_VOLTAGE_CONTROL   BATT_Feedback;
extern INT8U CANR_Pm25OutError; 
extern typedef struct  CAN_DRIVER    CANnode;


extern void  Input_Sensor_Proc(void);
extern void  Init_MCU_AD(void);
extern void  CAN_pro(void);
extern void  CAN_Init_RxMsg(void);
extern void  IGN_Feedback_Init(void);
extern void  BATT_Feedback_Init(void);
extern void  CAN_Init(void);
 void PM25In_Related_Out(void);


void     isr_high(void);

void Init_MCU_Clock(void)
{

OSCTUNEbits.PLLEN=0;

}

void Init_MCU_Timer(void)
{

PMD1bits.TMR1MD=0;       //enable the timer1
T1GCON    = 0X00;  
T1CON     = 0X06;     
   
RCONbits.IPEN   = 1;  
PIE1bits.TMR1IE = 1;   
IPR1bits.TMR1IP = 1;  
INTCONbits.GIEH =1;  
T1CONbits.TMR1ON =1;

}

void Init_interrupt(void)
 {
   INTCONbits.GIEL =1;
   INTCONbits.RBIE =1;
  // IOCBbits.IOCB4  =1;
   IOCBbits.IOCB5  =1;
 }
void Init_MCU_IO(void)
{
  TRISA = 0xff;
  TRISB = 0xfd;
  TRISC = 0x27;   //0x37;     //0x3f;  for can_enable no controlling
  PORTA = 0x00;
  PORTB = 0x00;
  PORTC = 0x00;
}

void CAN_Time_Init()
{ 

    OSCTUNEbits.PLLEN = 1;
    OSCCONbits.IRCF =   7;      //add

}

void InitCAN_Init(void)
{
    CANR_SIDH = 0x5fc >>3;  // ID_CANRX_MSG_Pm25Out>>3;
    CANR_SIDL = 0x5fc <<5; //ID_CANRX_MSG_Pm25Out<<5;
    TRISC = (TRISC | 0X80) & 0Xbf;   
    CANCON = 0x80;                        //请求配置模式
    while(!(CANSTATbits.OPMODE ==0x04));  //确认CAN处于配置模式
    //CIOCON =  0X00;                       //CAN I/O口控制寄存器，设置PLL时钟

        ECANCON= 0x00;                             
        CIOCON=0x21;                                         
   
	    BRGCON1=0X00;			  	          
        BRGCON2=0x98;    //0X90;				        
        BRGCON3=0x41;    //0X42;	
        		             
        BSEL0=0x00;                           
        RXB0CON = 0x20;    //0B00100000;              
        RXB1CON = 0x00;           

        RXB0SIDL=CANR_SIDH;
	    RXB0SIDH=CANR_SIDL & (0xE0);	
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
        
        
        RXF0SIDH = CANR_SIDH;  //0x67; 	  	          
        RXF0SIDL = CANR_SIDL & (0xE0);  //0xE0; 	
         
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
       CAN_Init_RxMsg(); 

   
}


void CAN_Transmit(unsigned int Duty,unsigned int Value,unsigned char Degree)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //发送缓冲寄存器
     TXB0SIDH =  CAN_ID>>3;                    //发送缓冲区标准标识符
     TXB0SIDL =  (CAN_ID<<5)%256;         
     TXB0DLC  =  0x08;                    //发送缓冲区数据长度
     TXB0D0   =  0xFF;                    //发送缓冲区0数据字段字节0
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST

     TXB0D1   =  0x3E&num<<1;
     TXB0D1   =  TXB0D1|(0xc0&Value<<6);
     TXB0D0   =  0xff&Value>>2;
     TXB0D3   =  0x80&Duty<<7;
     TXB0D3   |=  0xff&Degree<<3;      //add the Level display 2018.08.12
     TXB0D2   =  0xff&Duty>>1;
     TXB0D1   |=  0x01&Duty>>9;
     TXB0D6   =  0x00;
     TXB0D7   =  0x00;
     TXB0CON  =  TXB0CON | 0x08;          //请求发送报文
}


void LPWM_Dector(void)
{
   unsigned char i,k;   
   if(Buffer_Flag_Old!=Buffer_Flag)
     {
       LPWM_SUM =0;
       for(i=0;i<60;i++)
        {
         LPWM_SUM+=LPWM_Duty_Buffer[i];
        }
            
       LPWM_Duty =(float)LPWM_SUM*100/300;         //LPWM_SUM/30000*1000; 
//*
       if(CNT_Duty < AVG_LEN)
       {
         LPWM_Buffer[CNT_Duty] = LPWM_Duty;
         LPWM_Total = 0;
         for(k=0;k<CNT_Duty;k++)
         {
           LPWM_Total +=LPWM_Buffer[k];
         }
         LPWM_AVG=(float)LPWM_Total/CNT_Duty;
       }
       else
//*/
       {
         CNT_Duty = AVG_LEN;
         LPWM_Total = LPWM_Duty;
         for(k=0;k<(AVG_LEN-1);k++)
         {
           LPWM_Total +=LPWM_Buffer[k];
           LPWM_Buffer[k] = LPWM_Buffer[k+1];
         }
         LPWM_Buffer[AVG_LEN-1] = LPWM_Duty;
         LPWM_AVG=(float)LPWM_Total/AVG_LEN;    //CNT_Duty;
       }
       CNT_Duty +=1;
       LPWM_Duty = LPWM_AVG;    

       Buffer_Flag_Old = Buffer_Flag;
       //CAN_Transmit();
      }
         
     // LPWM_Duty=203;            // debugger for 20.5%  ,and 10.5 
    if (LPWM_Duty>=5000)  {LPWM_Duty=5000;}         //Remove the peaking Value
}


void LPWM_PM25Value(void)
{
     INT32U  temp;
      //***********************************************         
      // DISPLAY DELAY 3--5S 
      //****************************************
      /*
 	       
	    LPWM_Display=(float)(LPWM_Duty*10/2+LPWM_Duty_old*10/2)/10;
	       
	    Display_Counter=0;
	      
       */  

	   LPWM_Display=LPWM_Duty;
  
	   //**********************************
	  // calibration by PPD42  1.2  
	   /********************************** 
        if(LPWM_Display<=190)       temp = (float)LPWM_Display*22;
        else if(LPWM_Display<=380)  temp = (float)LPWM_Display*19+400;
        else if(LPWM_Display<=570)  temp = (float)LPWM_Display*19+400;
        else if(LPWM_Display<=730)  temp = (float)LPWM_Display*21-600;
        else if(LPWM_Display<=880)  temp = (float)LPWM_Display*23-2100;
        else if(LPWM_Display<=1020) temp = (float)LPWM_Display*26-4200;
        else if(LPWM_Display<=1140) temp = (float)LPWM_Display*32-10300;
        else if(LPWM_Display<=1260) temp = (float)LPWM_Display*34-13200;
        else if(LPWM_Display<=1360) temp = (float)LPWM_Display*41-21800;
        else if(LPWM_Display<=1520) temp = (float)LPWM_Display*46-28100;
        else if(LPWM_Display<=1670) temp = (float)LPWM_Display*53-39800;
        else if(LPWM_Display<=1760) temp = (float)LPWM_Display*64-58300;
        else if(LPWM_Display<=2230) temp = (float)LPWM_Display*49-29900;
        else if(LPWM_Display<=2500) temp = (float)LPWM_Display*96-13480;
        //else if(LPWM_Duty<2700)    temp = (float)PWM_Duty*60-30800;
        //else if(LPWM_Duty<2800)    temp = (float)LPWM_Duty*100-140000;
        else if(LPWM_Display>=2500) temp=110000;
        if(PM25_Value>110000)       temp=110000;
 
        PM25_Value = temp/100;
        LPWM_Display = LPWM_Display/10; 
        
       //********************************/ 
      
	        
	  //*******************
      //for inlet PM2.5 sensor
      /********************   
        
        if(LPWM_Display <= 100)       temp = (float)LPWM_Display*35;
        else if(LPWM_Display <= 130)  temp = (float)LPWM_Display*33+200;
        else if(LPWM_Display <= 230)  temp = (float)LPWM_Display*11+3100;
        else if(LPWM_Display <= 460)  temp = (float)LPWM_Display*6+4200;
        else if(LPWM_Display <= 580)  temp = (float)LPWM_Display*26-4900;
        else if(LPWM_Display <= 860)  temp = (float)LPWM_Display*37-11400;
        else if(LPWM_Display <= 1200) temp = (float)LPWM_Display*50-22800;
        else if(LPWM_Display <= 1390) temp = (float)LPWM_Display*118-103900;
        else if(LPWM_Display <= 1680) temp = (float)LPWM_Display*38+7300;
        else if(LPWM_Display <= 1950) temp = (float)LPWM_Display*52-16100;
        else if(LPWM_Display <= 2200) temp = (float)LPWM_Display*69-49900;
        else if(LPWM_Display <= 2360) temp = (float)LPWM_Display*48-3600;
        else if(LPWM_Display <= 2500) temp = (float)LPWM_Display*71-58600;
        //else if(LPWM_Duty<2700)     temp =LPWM_Duty*60-30800;
        //else if(LPWM_Duty<2800)     temp =LPWM_Duty*100-140000;
        else temp =120000;
        if( temp >120000)  temp = 120000;
   
        PM25_Value = temp/100;
        LPWM_Display = LPWM_Display/10; 
        
        //**************/

       //*****************
       // for outlet pm25 sensor  
       /**************  
        
        if(LPWM_Display <= 100)       temp = (float)LPWM_Display*50;
        else if(LPWM_Display <= 130)  temp = (float)LPWM_Display*47+300;
        else if(LPWM_Display <= 230)  temp = (float)LPWM_Display*16+4300;
        else if(LPWM_Display <= 460)  temp = (float)LPWM_Display*7+6400;
        else if(LPWM_Display <= 580)  temp = (float)LPWM_Display*40-8800;
        else if(LPWM_Display <= 860)  temp = (float)LPWM_Display*40-8800;
        else if(LPWM_Display <= 1200) temp = (float)LPWM_Display*63-28500;
        else if(LPWM_Display <= 1390) temp = (float)LPWM_Display*68-35100;
        else if(LPWM_Display <= 1680) temp = (float)LPWM_Display*38+7300;
        else if(LPWM_Display <= 1950) temp = (float)LPWM_Display*52-16100;
        else if(LPWM_Display <= 2200) temp = (float)LPWM_Display*69-49900;
        else if(LPWM_Display <= 2360) temp = (float)LPWM_Display*48-3600;
        else if(LPWM_Display <= 2500) temp = (float)LPWM_Display*71-58600;
        //else if(LPWM_Duty<2700)     temp =LPWM_Duty*60-30800;
        //else if(LPWM_Duty<2800)     temp =LPWM_Duty*100-140000;
        else temp =120000;
        if( temp >120000)  temp = 120000;
   
        PM25_Value = temp/100;
        LPWM_Display = LPWM_Display/10; 

       //*****************/
        
       /*****************
       // for test 2017.10.23 
       //**************  
        
        if(LPWM_Display <= 100)       temp = (float)LPWM_Display*50;
        else if(LPWM_Display <= 130)  temp = (float)LPWM_Display*33+700;
        else if(LPWM_Display <= 230)  temp = (float)LPWM_Display*10+3700;
        else if(LPWM_Display <= 460)  temp = (float)LPWM_Display*4+5000;
        else if(LPWM_Display <= 580)  temp = (float)LPWM_Display*17-700;
        else if(LPWM_Display <= 860)  temp = (float)LPWM_Display*11+2800;
        else if(LPWM_Display <= 1200) temp = (float)LPWM_Display*18-3200;
        else if(LPWM_Display <= 1390) temp = (float)LPWM_Display*63-57800;
        else if(LPWM_Display <= 1680) temp = (float)LPWM_Display*52-41900;
        else if(LPWM_Display <= 1950) temp = (float)LPWM_Display*93-110600;
        else if(LPWM_Display <= 2200) temp = (float)LPWM_Display*40-8000;
        else if(LPWM_Display <= 2360) temp = (float)LPWM_Display*156-236800;
        else if(LPWM_Display <= 2500) temp = (float)LPWM_Display*71-63600;
        //else if(LPWM_Duty<2700)     temp =LPWM_Duty*60-30800;
        //else if(LPWM_Duty<2800)     temp =LPWM_Duty*100-140000;
        else temp =120000;
        if( temp >120000)  temp = 120000;
   
        PM25_Value = temp/100;
       
        LPWM_Display = LPWM_Display/10; 

       //*****************/

       //*****************
       //Review 2017.10.27 
       //**************  
        
        if(LPWM_Display <= 100)       temp = (float)LPWM_Display*40;
        else if(LPWM_Display <= 130)  temp = (float)LPWM_Display*33+700;
        else if(LPWM_Display <= 230)  temp = (float)LPWM_Display*10+3700;
        else if(LPWM_Display <= 460)  temp = (float)LPWM_Display*4+5000;
        else if(LPWM_Display <= 580)  temp = (float)LPWM_Display*17-700;
        else if(LPWM_Display <= 860)  temp = (float)LPWM_Display*11+2800;
        else if(LPWM_Display <= 1200) temp = (float)LPWM_Display*18-3200;
        else if(LPWM_Display <= 1390) temp = (float)LPWM_Display*63-57800;
        else if(LPWM_Display <= 1680) temp = (float)LPWM_Display*52-41900;
        else if(LPWM_Display <= 1950) temp = (float)LPWM_Display*70-73200;
        else if(LPWM_Display <= 2200) temp = (float)LPWM_Display*64-60800;
        else if(LPWM_Display <= 2360) temp = (float)LPWM_Display*100-140000;
        else if(LPWM_Display <= 2500) temp = (float)LPWM_Display*136-224300;
        //else if(LPWM_Duty<2700)     temp =LPWM_Duty*75-72500;
        //else if(LPWM_Duty<2800)     temp =LPWM_Duty*100-140000;
        else temp =120000;
        if( temp >120000)  temp = 120000;
   
        PM25_Value = temp/100;
       
        LPWM_Display = LPWM_Display/10; 

       //*****************/

}


void Display_Handle(void)
{
       
        //************************************************
        //This code for controling the PM25 changing speed.
        //Date:2017.7.27
        //*************************************************
        //PM25_Inc=PM25_Value_old/10+1;
        
        if ((PM25_Value - PM25_Value_old) == 0) PM25_Display =PM25_Value_old;
        else
        {
          if (PM25_Value>PM25_Value_old)
            {
	          //PM25_Bias=PM25_Value-PM25_Value_old;
	          //if (PM25_Bias>=PM25_Inc) PM25_Display=PM25_Value_old+PM25_Inc;
	          //else PM25_Display=PM25_Value_old+PM25_Bias;
	        
	          PM25_Bias = (PM25_Value-PM25_Value_old)/10+1;
              PM25_Display = PM25_Value_old+PM25_Bias;
	         }
	       else
	        {
		      //PM25_Bias=PM25_Value_old-PM25_Value;
		      //if (PM25_Bias>=PM25_Inc) PM25_Display=PM25_Value_old-PM25_Inc;
		      //else PM25_Display=PM25_Value_old-PM25_Bias;
		      PM25_Bias =(PM25_Value_old-PM25_Value)/10+1;
              PM25_Display = PM25_Value_old-PM25_Bias;
		    } 
          } 
		  
         // PM25_Display=PM25_Value;
        
		  if (PM25_Display>=1023)  PM25_Display=1023;
		  
	    //***************************************************/
     
	     if(PM25_Display<=50)
         {
	        if (Level_old <=1)                          Level = 1;
	        else if (Level_old == 2)
	              {
	               if  (PM25_Display<=45)               Level = 1;
	               else if (PM25_Display >45 )          Level = 2;
	              }
	        else  if (Level_old > 2 )                   Level = Level_old - 1;
	     }
        else if((PM25_Display>=51 )&(PM25_Display<=100))  
         {
	        if (Level_old<=1)
	           {
		         if (PM25_Display<=55)                Level =1;
		         else if (PM25_Display > 55 )         Level =2;
		       }
		    else if (Level_old == 2)                  Level =2;
		    else if (Level_old == 3)
		           {
			           if (PM25_Display <=95)         Level =2;
			           else if (PM25_Display >95)     Level =3;
			       }
			else if (Level_old >3 )                   Level = Level_old-1; 
	        
	     }
        else if((PM25_Display>=101)&(PM25_Display<=150)) 
        {
	        if (Level_old <=1)                       Level = Level_old +1;
	        else if (Level_old ==2)
	              {
		              if (PM25_Display <=105)        Level = 2;
		              else if (PM25_Display >105)    Level =3;
		          }
		    else if (Level_old ==3 )                 Level =3;
		    else if (Level_old ==4 )
		          {
			          if (PM25_Display <=145 )       Level =3;
			          else if (PM25_Display >145)    Level =4;
			      }
			else if  (Level_old >4)                  Level = Level_old - 1;
			
	      
	     }
        else if((PM25_Display>=151)&(PM25_Display<=200))
        {
	        if (Level_old <=2)                      Level = Level_old +1;
	        else if (Level_old ==3)
	              {
		              if (PM25_Display <=155)       Level = 3;
		              else if (PM25_Display >155)   Level =4;
		          }
		    else if (Level_old ==4 )                Level =4;
		    else if (Level_old ==5 )
		          {
			          if (PM25_Display <=195 )      Level =4;
			          else if (PM25_Display >195)   Level =5;
			      }
			else if  (Level_old >5)                 Level = Level_old - 1; 
	     }
        else if((PM25_Display>=201)&(PM25_Display<=300)) 
        {
	       if (Level_old <=3)                       Level = Level_old +1;
	        else if (Level_old ==4)
	              {
		              if (PM25_Display <=205)       Level = 4;
		              else if (PM25_Display >205)   Level =5;
		          }
		    else if (Level_old ==5 )                Level =5;
		    else if (Level_old ==6 )
		          {
			          if (PM25_Display <=295 )      Level =5;
			          else if (PM25_Display >295)   Level =6;
		          }
	     }
        else if(PM25_Display>=301) 
        {
	        if (Level_old <=4)                      Level = Level_old +1;
	        else if (Level_old ==5)
	              {
		              if (PM25_Display <=305)       Level = 5;
		              else if (PM25_Display >305)   Level =6;
		          }
		    else if (Level_old ==6 )                Level =6;
		    
	     }
        if (Level>=6) Level =6;
}


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
   if(PIR1bits.TMR1IF==1)
    {  
      PIR1bits.TMR1IF=0;      
      TMR1H=C_TIMER_1MS/256;		       
      TMR1L=C_TIMER_1MS%256;
      if(LPWM_SUM_timer)      LPWM_SUM_timer--;
      else
        {
          LPWM_SUM_timer=500;
          CAN_Timer =1;
          Buffer_Flag++;
          if(Buffer_Flag>=60)  Buffer_Flag =0;                  //  ORG:60   Buffer_Flag>60
          //if (LPWM_Timer_Count>200) LPWM_Timer_Count=200;    //Code:20161127
          LPWM_Duty_Buffer[Buffer_Flag] =LPWM_Timer_Count;
          LPWM_Timer_Count=0;
          Display_Counter++;                                 //code:20161208
        }
      if(LPWM_Timer_Flag==1)  LPWM_Timer_Count++;
        
    } 
   if(INTCONbits.RBIF == 1)
    {
       INTCONbits.RBIF =0;
       if(PORTBbits.RB5 == 0) 
         LPWM_Timer_Flag =1;
       else LPWM_Timer_Flag =0;
       
    }
   if(PIR5bits.RXB0IF==1)
	{
  		CANR_FLAG=1;				//* 置接收到 CAN 数据 */
		PIR5bits.RXB0IF=0;					//* 清接收中断标志 */
		RXB0CONbits.RXFUL=0;					//* bit7＝0,打开接收缓冲器接收新信息 */
    }
}
#pragma code    /*end the program  */

void PM25In_Related_Out(void)
{
 if( CANR_OUT_OK  ==1  )       //C_Pm25Out_OK
 {
   //PIE5bits.RXB0IE=0;
   //PM25_Value = CANR_Pm25OutValue;
    //PM25_Value =36;              // These code for test
    //CANR_Pm25OutValue =20;

  if ((( CANR_Pm25OutValue - PM25_Value ) > 0 ) & ((CANR_Pm25OutValue - PM25_Value) <= 5))
   {
      if (CANR_Pm25OutValue >5)
      {
        PM25_Value = CANR_Pm25OutValue - 5 ;
      }
      else {;}
   }  
  else {;}

if ( PM25_Value > CANR_Pm25OutValue  )  //(( PM25_Value - CANR_Pm25OutValue) > 0 )
  {
    if ((PM25_Value - CANR_Pm25OutValue ) < 3)
        {
          if(PM25_Value >3)
          {     
           PM25_Value  = (PM25_Value - 3);
          }
          else {;}
        }
    else if (((PM25_Value - CANR_Pm25OutValue) >= 3) & ((PM25_Value - CANR_Pm25OutValue) <= 15))
     {  
        if(PM25_Value > 8)
        { 
          PM25_Value = PM25_Value - 8;
        }
        else 
         {PM25_Value = CANR_Pm25OutValue ;}    
     }
      else if((PM25_Value - CANR_Pm25OutValue) > 15)
        {
         
           PM25_Value = PM25_Value - 15;
        }
        //else {;}
   }
   else {
           ;
        }

 //PIE5bits.RXB0IE=1;
 }
 else {;}

  //PM25_Value = CANR_Pm25OutValue;

}

void main(void)
{
   unsigned char j;
   unsigned int Power_CNT;

   INT8U test;
   CAN_EN=1;    // for can_enable controlling
   Power_CNT=0;
   LPWM_SUM_timer=500;
   LPWM_Duty =0;
   PM25_Value =0;
   PM25_Value_old=0;
   PM25_Display=0;
   LPWM_Display=0;
   LPWM_Duty_old=0;
   LPWM_Timer_Flag =0;
   LPWM_Timer_Count=0;
   Display_Counter_Old=Display_Counter;
   CAN_Timer=0;
   Level = 0;
   Level_old =0;
   CANR_Pm25OutValue=0;
   CANR_OUT_OK =0;
   
   Init_MCU_Clock();
   Init_MCU_Timer();
   Init_MCU_IO();
   //CAN_Time_Init();
   InitCAN_Init();
   Init_interrupt();
   Init_MCU_AD();
   IGN_Feedback_Init();
   BATT_Feedback_Init();
   CAN_Init_RxMsg();
   CAN_Init();
   CAN_EN=0;    // for can_enable controlling
   WDTCON = 0x01;
   while(1)
    {
       //CAN_pro( );
       POWER=1;
       
       Input_Sensor_Proc();
       test=IGN_Feedback.Status;
       
       LPWM_Dector();
       LPWM_PM25Value(); 

       CAN_pro();
       PM25In_Related_Out();
 
       Display_Handle();

//     PM25_Display = CANR_Pm25OutValue;
	      
	   test=IGN_Feedback.Status;
  
          if((CAN_Timer)&(IGN_Feedback.Status==C_IGN_NORMAL)&(BATT_Feedback.Status==C_IGN_NORMAL)) 
             
  //      if((CAN_Timer)&(BATT_Feedback.Status==C_IGN_NORMAL))
	      {
           CAN_send_flag++;
           if(CAN_send_flag==1)
	         {
             num++;
             if(num>29)num=0;
             
             //****************
             if (Power_CNT <=2)
	         {
		      Power_CNT=Power_CNT+1;
		      PM25_Display =0;             //PM25_Value;
		     }
		     //************************/
		     
            // CAN_Transmit(LPWM_Duty,PM25_Value);
             CAN_Transmit(LPWM_Display,PM25_Display,Level);
            // PM25_Value_old=PM25_Value;
             PM25_Value_old=PM25_Display;
            // LPWM_Duty_old=LPWM_Duty;
             LPWM_Duty_old=LPWM_Display;
             Level_old = Level;
            }
           else if (CAN_send_flag==2) 
            {
               CAN_Transmit(LPWM_Duty_old,PM25_Value_old,Level_old);
               CAN_send_flag=0;
            }
             // while(TXB0CONbits.TXBIF!=1);
             CAN_Timer=0;
           }
         test=IGN_Feedback.Status;   
    }
}