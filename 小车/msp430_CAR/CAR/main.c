#include <msp430f5529.h>
#include <math.h>
#include "LDC1000_cmd.h"
#include "ocmj48c.h"
#include "RENCODE.h"

#define Range_Min  9300
#define Range_Max  12000
#define maxleft 1
#define midleft 2
#define minleft 3

#define maxright 4
#define midright 5
#define minright 6

#define middle   7
unsigned char buff_p[6];
unsigned char buff_dis[14]="dis:000.0cm ";
unsigned char time[]="time: 00:00:00";
unsigned char sec,min,hour;
unsigned char time_flag;
unsigned int t1ms;
unsigned long int dis;
unsigned char max_num;
unsigned char type;
//int buff[10];
unsigned  int buff_t[10];
unsigned char car_status;

int proximtyData_array[50];

unsigned char mid_to_edge,edge_to_mid;
void SetVCoreUp(unsigned int level);
unsigned int num;
void Init_Clk(void);

char spi_readByte( char addr, char * data);
char spi_readWord(char addr, unsigned int * data);  // Big Endian
char spi_readBytes( char addr, char * buffer, unsigned char len);
char spi_writeByte(char addr, char data);
char spi_writeWord(char addr, unsigned int data);  // Big Endian
char spi_writeBytes( char addr, char * buffer, unsigned char len);

static unsigned char txlen;
static unsigned char rxlen;
static char *txbuf;
static char *rxbuf;
static char txaddr;
static char wordbuf[2];

char proximtyData[2];
char frequencyData[3];

int proximtyDataMIN;
int frequencyDataMIN;

int proximtyDataMAX;
int frequencyDataMAX;

unsigned int proximtyDataTEMP;
int frequencyDataTEMP;
extern void delayms(uint xms);
extern void inter_init();
extern void dididi(void);

#define RPMIN 0x3A
#define RPMAX 0x13

/** @} */
/*
 * main.c
 */

//PWM
void TA0_init()
{
    TA0CTL|=TASSEL_2+TACLR+MC0+ID_3;
    TA0CCR0=60000;
    TA0CCR2=3000;  //(1600~7400)
    TA0CCR3=25000;  
    TA0CCR4=25000;
    TA0CCTL2|= OUTMOD_7;
    TA0CCTL3|= OUTMOD_7;   
    TA0CCTL4|= OUTMOD_7;
    P1SEL |=BIT4;P1DIR |=BIT4;
    P1SEL |=BIT5;P1DIR |=BIT5;
}
void TA2_init()
{
      TA2CTL|=TASSEL_2+TACLR+TAIE+ID_3;  //
      TA2CCR0=3125;                  //1KHz
}

#pragma vector=TIMER2_A1_VECTOR
__interrupt void time0()
{
      switch(TA2IV)
      {
      case 2:break;
      case 4:break;
      case 14:
      t1ms++;
      if(t1ms==1000)
      {
        t1ms=0;
        sec+=1;
        time_flag=1;
       
      
        if(sec==60)
        {
          sec=0;     

          
          min+=1;
          if(min>59)
          {
            min=0;
            hour+=1;
            if(hour==24)
            {
              hour=0;
            }
          }
        }      
        
      }   break;///归零函数TR1=0;TH1=0;TL1=0;
    }
}

//光电传开关中断记里程
//#pragma vector=PORT2_VECTOR
//
//__interrupt void PORT2(void)
//{
//  if(P2IFG&BIT0)
//  {
//    P2IFG&=~BIT0; 
//    num++;   
//    dis=num*107;
//  }
//}

void max_left()
{
    TA2CTL|=MC0;
    P6OUT &=~BIT1;P6OUT|=BIT0; //左后退
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR3=23000; //20000 
    TA0CCR4=30000;//50000
}
void mid_left()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR3=14000;  
    TA0CCR4=28000;//28000
}
void min_left()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR3=15000;  
    TA0CCR4=24000;//24000
}
void max_right()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT &=~BIT3;P6OUT |=BIT2; //右后退
    TA0CCR4=23000; //20000 
    TA0CCR3=30000;//50000
}
void mid_right()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR4=14000;  
    TA0CCR3=28000;//28000
}
void min_right()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR4=15000;  
    TA0CCR3=24000;//24000
}
void go_ahead()
{
    TA2CTL|=MC0;
    P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
    P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
    TA0CCR3=25000;  
    TA0CCR4=25000;
}
void back(void)
{
    P6OUT |=BIT0;P6OUT &=~BIT1; //后退
    P6OUT |=BIT2;P6OUT &=~BIT3; //后退
    TA2CTL&=~MC0;
    TA0CCR3=30000;  
    TA0CCR4=30000;
}

//循迹
void Go(void)
{ 
    int j;
    unsigned char aa[4];

    int max=proximtyData_array[0];
    for(j=0;j<50;j++)
    {
	if(proximtyData_array[j]>=max)
	{
		max = proximtyData_array[j];
		max_num = j;
	}
    }
    aa[0]=max_num/10+0x30;aa[1]=max_num%10+0x30;aa[2]=0;aa[3]=0;
    //disp_chars(0,2,aa);
   
    if(((max>28000)&&(type==1))||((max>26000)&&(type==0)) ) //找到硬币
    {
        disp_chars(3,3,"找到硬币");
        dididi();
    }
    else   disp_chars(3,3,"          ");
    if(((max>24800)&&(type==1))||((max>23200)&&(type==0))) //找到了金属线
    {
      if (max_num>25) 
      {
        if(max_num>36) car_status=maxleft; 
        else if((max_num>30))car_status=midleft; 
        else car_status=minleft; 
        disp_chars(0,3,"偏左");
      }
      else if(max_num>23)
      {
        car_status=middle; 
        disp_chars(0,3,"中位");
      }
      else
      {  
        if(max_num<13) car_status=maxright;
        else if((max_num<19))car_status=midright;
        else  car_status=minleft;
        disp_chars(0,3,"偏右");
      }
    }
    else
    {
      car_status=0;//跑偏
      disp_chars(0,3,"跑偏");
    }
    switch(car_status)
    {
      case maxleft: max_right();break;
      case midleft: mid_right();break;
      case minleft: min_right();break;
      case maxright: max_left();break;
      case midright: mid_left();break;
      case minright: min_left();break;
      case middle  : go_ahead();break;
      case 0 :back();delayms(200);break;
      default: break;
    }    
}

void dididi(void)
{
      P6OUT |=BIT6;
      delayms(500);
      P6OUT &=~BIT6;
      delayms(500);
      P6OUT |=BIT6;
      delayms(500);
      P6OUT &=~BIT6;
      delayms(500);
      P6OUT |=BIT6;
      delayms(500);
      P6OUT &=~BIT6;
}
void dis_time(void)
{
   if(dis_flag)
              {
                dis_flag=0;
              //显示距离
                buff_dis[4]=dis/1000%10+0x30;
                buff_dis[5]=dis/100%10+0x30;
                buff_dis[6]=dis/10%10+0x30;
                buff_dis[8]=dis%10+0x30;
                //buff_dis[6]=dis/10%10+0x30;
                //buff_dis[7]=dis%10+0x30;
                //disp_chars(0,1,"dist");
                disp_chars(0,1,buff_dis);
              }
              if(time_flag)
              {
                time_flag=0;
              //显示时间
                time[6]=hour/10+0x30;
                time[7]=hour%10+0x30;          
                time[9]=min/10+0x30;
                time[10]=min%10+0x30;         
                time[12]=sec/10+0x30;
                time[13]=sec%10+0x30;  
                disp_chars(0,2,time); 
              }

}
char main(void) 
{
      unsigned char i;
      
      WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
      //int last_proximtyDataTEMP;
      
      Init_Clk();
      lcd_init();
      TA0_init();
      TA2_init();
      TA1_init();
      inter_init();
      _EINT();
      
//蜂鸣器
      P6DIR |=BIT6;
      P6OUT &=~BIT6;
//PORT INIT
      P1DIR |= BIT0;                        // LDC CLK for Freq counter (set to output selected clock)
      P1SEL |=BIT0;
      P3DIR |=BIT0+BIT1;
      P3OUT &=~(BIT0+BIT1);
      P6DIR |=BIT0+BIT1+BIT2+BIT3;
//舵机      
      P1DIR |= BIT3;                        
      P1SEL |=BIT3;
//光电开关
      P2DIR&=~BIT0;       
      P2REN|=BIT0;  
      P2OUT |=BIT0;
      P2IES|=BIT0;
      P2IE|=BIT0;

//	INTB INIT
//	P1DIR &= ~BIT2;                            // Set P1.2 input
//	P1IES |= BIT2;                           // P1.2 Hi/Lo edge
//	P1IFG &= ~BIT2;                           // P1.2 IFG cleared
//	P1IE |= BIT2;                             // P1.2 interrupt enabled

// initialize SPI
     P4DIR |= BIT0;  // Output
     P4SEL &= ~BIT0;
	
//SPI SETUP
     P4SEL |=BIT1 + BIT2 + BIT3;
     UCB1CTL1 |=UCSWRST;
     UCB1CTL0 |= UCMST+UCMSB+UCSYNC+UCCKPL;   // 3-pin, 8-bit SPI master,Clock polarity high, MSB
     UCB1CTL1 |= UCSSEL_1;                 // CLOCK ACLK
     UCB1BR0 = 0x06;
     UCB1BR1 = 0;
     UCB1CTL1 &= ~UCSWRST;

/*****************TEST*********TEST***************TEST*********/

//read all REG value using default setting
     char orgVal[20];

//write to register
    spi_writeByte(LDC1000_CMD_RPMAX,       RPMAX);
    spi_writeByte(LDC1000_CMD_RPMIN,       RPMIN);
    spi_writeByte(LDC1000_CMD_SENSORFREQ,  0x94);
    spi_writeByte(LDC1000_CMD_LDCCONFIG,   0x17);
    spi_writeByte(LDC1000_CMD_CLKCONFIG,   0x02);
    spi_writeByte(LDC1000_CMD_INTCONFIG,   0x02);

    spi_writeByte(LDC1000_CMD_THRESHILSB,  0x50);
    spi_writeByte(LDC1000_CMD_THRESHIMSB,  0x14);
    spi_writeByte(LDC1000_CMD_THRESLOLSB,  0xC0);
    spi_writeByte(LDC1000_CMD_THRESLOMSB,  0x12);

    spi_writeByte(LDC1000_CMD_PWRCONFIG,   0x01);

  //read all registers

    spi_readBytes(LDC1000_CMD_REVID, &orgVal[0],12);

    
    //read all registers using extended SPI
    spi_readBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
        spi_readBytes(LDC1000_CMD_FREQCTRLSB,&frequencyData[0],3);

        proximtyDataMAX = ((unsigned char) proximtyData[1]<<8) + proximtyData [0];
        frequencyDataMAX = ((unsigned char)frequencyData[1]<<8) + frequencyData[0];

        proximtyDataMIN = proximtyDataMAX;
        frequencyDataMIN = frequencyDataMAX;

    	for (i=0;i<100;i++)
    	{
          spi_readBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
          spi_readBytes(LDC1000_CMD_FREQCTRLSB,&frequencyData[0],3);

          proximtyDataTEMP = ((unsigned char)proximtyData[1]<<8) + proximtyData [0];
          frequencyDataTEMP = ((unsigned char)frequencyData[1]<<8) + frequencyData[0];

          if (proximtyDataTEMP < proximtyDataMIN)
              proximtyDataMIN = proximtyDataTEMP;
          if (frequencyDataTEMP < frequencyDataMIN)
              frequencyDataMIN = frequencyDataTEMP;

          if (proximtyDataTEMP > proximtyDataMAX)
              proximtyDataMAX = proximtyDataTEMP;
          if (frequencyDataTEMP > frequencyDataMAX)
              frequencyDataMAX = frequencyDataTEMP;
    	}
        
      P6OUT |=BIT1;P6OUT &=~BIT0; //左前进
      P6OUT |=BIT3;P6OUT &=~BIT2; //右前进
      delayms(1000);
      P6OUT &=~BIT1;P6OUT &=~BIT0; //停车
      P6OUT &=~BIT3;P6OUT &=~BIT2; //停车
    spi_readBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
    proximtyDataTEMP = ((unsigned char)proximtyData[1]<<8) + proximtyData [0]; 
    
    if(proximtyDataTEMP>23500) type=1;
    else type=0;
    
    while(key!=0xa2) ;
    TA2CTL|=MC0;
    while (1)
    {
       if(key!=0xff)
       {
         
        }
        
          P6OUT |=BIT1;P6OUT &=~BIT0; //停车
          P6OUT |=BIT3;P6OUT &=~BIT2; //停车
          TA0CCR3=9000;  
          TA0CCR4=9000;
          for(i=0;i<50;i++)
          {
            //传感器数值
              spi_readBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
              proximtyDataTEMP = ((unsigned char)proximtyData[1]<<8) + proximtyData [0];    
              proximtyData_array[i]=proximtyDataTEMP;
              TA0CCR2+=60;  
              buff_p[0]=proximtyDataTEMP/10000%10+0x30;
              buff_p[1]=proximtyDataTEMP/1000%10+0x30;
              buff_p[2]=proximtyDataTEMP/100%10+0x30;
              buff_p[3]=proximtyDataTEMP/10%10+0x30;
              buff_p[4]=proximtyDataTEMP%10+0x30;       
              disp_chars(0,0,buff_p); 
              if(i==5) 
              {
                P6OUT &=~BIT1;P6OUT &=~BIT0; //停车
                P6OUT &=~BIT3;P6OUT &=~BIT2; //停车
              }
              delayms(20);             
          }
          dis_time();
          Go();          
          delayms(1000);          
          TA0CCR2=6000;
           //P6OUT &=~BIT1;P6OUT &=~BIT0; //停车
           //P6OUT &=~BIT3;P6OUT &=~BIT2; //停车
          P6OUT |=BIT1;P6OUT &=~BIT0; //停车
          P6OUT |=BIT3;P6OUT &=~BIT2; //停车
          TA0CCR3=9000;  
          TA0CCR4=9000;
           for(i=49;i>0;i--)
          {
            //传感器数值
              spi_readBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
              proximtyDataTEMP = ((unsigned char)proximtyData[1]<<8) + proximtyData [0];    
              proximtyData_array[i-1]=proximtyDataTEMP;
              TA0CCR2-=60;  
              buff_p[0]=proximtyDataTEMP/10000%10+0x30;
              buff_p[1]=proximtyDataTEMP/1000%10+0x30;
              buff_p[2]=proximtyDataTEMP/100%10+0x30;
              buff_p[3]=proximtyDataTEMP/10%10+0x30;
              buff_p[4]=proximtyDataTEMP%10+0x30;       
              disp_chars(0,0,buff_p);  
              if(i==44) 
              {
                P6OUT &=~BIT1;P6OUT &=~BIT0; //停车
                P6OUT &=~BIT3;P6OUT &=~BIT2; //停车
              }              
              delayms(20);             
          }      
          __no_operation();
        
          dis_time();
          Go();          
          delayms(1000);        
          
          TA0CCR2=3000;          
       
      }
}


/**sub functions**/

char spi_readByte( char addr, char * data)
{
	    rxlen = 1;
		rxbuf = data;
		txaddr = addr | 0x80;

		P4OUT &= ~BIT0;
		while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = txaddr;
		while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = 0;
		while (UCB1STAT & UCBUSY);
		* rxbuf = UCB1RXBUF;

		while (UCB1STAT & UCBUSY);
		P4OUT |= BIT0;

		return 0;
}

char spi_readWord(char addr, unsigned int * data)
{
	rxlen = 2;
	rxbuf = &wordbuf[0];
	txaddr = addr | 0x80;

	P4OUT &= ~BIT0;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = txaddr;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = 0;
	while (UCB1STAT & UCBUSY);
	* rxbuf = UCB1RXBUF;
	rxbuf++;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = 0;
	while (UCB1STAT & UCBUSY);
	* rxbuf = UCB1RXBUF;

	while (UCB1STAT & UCBUSY);
	P4OUT |= BIT0;

	return 0;

}
char spi_readBytes( char addr, char * buffer, unsigned char len)
{
	rxlen = len;
	rxbuf = buffer;
	txaddr = addr | 0x80;

	P4OUT &= ~BIT0;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = txaddr;

	
        while (rxlen > 0) 
        {
		while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = 0;
		while (UCB1STAT & UCBUSY);
		* rxbuf = UCB1RXBUF;
		rxbuf++;
		rxlen--;
	}

	while (UCB1STAT & UCBUSY);
	P4OUT |= BIT0;

	return 0;
}

char spi_writeByte(char addr, char data)
{
	wordbuf[0] = data;          // copy from stack to memory
	txlen = 1;
	txbuf = &wordbuf[0];
	txaddr = addr & ~0x80;

	P4OUT &= ~BIT0;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = txaddr;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = *txbuf;

	while (UCB1STAT & UCBUSY);
	P4OUT |= BIT0;

	return 0;
}

char spi_writeWord(char addr, unsigned int data)
{
	wordbuf[0] = data >> 8;    // Big Endian
	wordbuf[1] = data & 0xFF;
	txlen = 2;
	txbuf = &wordbuf[0];
	txaddr = addr & ~0x80;

	P4OUT &= ~BIT0;
	while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = txaddr;
	while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = *txbuf;
		txbuf++;
	while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = *txbuf;

	while (UCB1STAT & UCBUSY);
	P4OUT |= BIT0;

	return 0;
}

char spi_writeBytes( char addr, char * buffer, unsigned char len)
{
	txlen = len;
	txbuf = buffer;
	txaddr = addr & ~0x80;

	P4OUT &= ~BIT0;
	while (!(UCB1IFG&UCTXIFG));
	UCB1TXBUF = txaddr;

	while (txlen > 0) 
        {
		while (!(UCB1IFG&UCTXIFG));
		UCB1TXBUF = *txbuf;
		txbuf++;
		txlen--;
        }
		while (UCB1STAT & UCBUSY);
		P4OUT |= BIT0;

		return 0;

}


void SetVCoreUp (unsigned int level)  
{  
  // Open PMM registers for write  
  PMMCTL0_H = PMMPW_H;  
  // Set SVS/SVM high side new level  
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;  
  // Set SVM low side to new level  
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;  
  // Wait till SVM is settled  
  while ((PMMIFG & SVSMLDLYIFG) == 0);  
  // Clear already set flags  
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);  
  // Set VCore to new level  
  PMMCTL0_L = PMMCOREV0 * level;  
  // Wait till new level reached  
  if ((PMMIFG & SVMLIFG))  
    while ((PMMIFG & SVMLVLRIFG) == 0);  
  // Set SVS/SVM low side to new level  
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;  
  // Lock PMM registers for write access  
  PMMCTL0_H = 0x00;  
}  

/*************************************************
  函数(模块)名称:void Init_Clk(void)
  功能:	时钟初始化
  本函数(模块)调用的函数(模块)清单:
  调用本函数(模块)的函数(模块)清单:
  输入参数:     void        	              		
  输出参数:     void       	
  函数返回值说明:  		
  使用的资源    		
  其它说明:       		
*************************************************/
void Init_Clk(void)
{
  /*****************************************************************************************************/ 
  /*******配置 MCLK = DCOC,SMCLK =DCOC,ACLK=XT1      8M  調試OK*****************************************/
  /****************************************************************************************************/
  /*  
     P5SEL |= BIT4|BIT5;  
     UCSCTL6 |= XCAP_3;  
     UCSCTL6 &= ~XT1OFF;   //XT1起振
     
     UCSCTL3 |= SELREF_2;  //将REFCLK配置为REFCLK (不设置的话默認為XT1，这里设置为REFO)
     
     UCSCTL1 = DCORSEL_4;                             // 选择FLL参考源自REFO  
     UCSCTL2 |= 0x79;                                 // Set DCO Multiplier for 8MHz
     UCSCTL4 = SELM_3 + SELA_0 + SELS_3;              // 配置 MCLK = DCOC,SMCLK =DCOC,ACLK=XT1
     UCSCTL5|= DIVS_3;                                // SMCLK 由DCO 8分頻而來，即为1MHz
     while (SFRIFG1 & OFIFG)                          //清除OFIFG,and  XT1OFFG ,DCOFFG
     {
          UCSCTL7 &= ~(  XT1LFOFFG + DCOFFG);
          SFRIFG1 &= ~OFIFG;
     } */
  
  /***************************************************************************************************/
     /****************将SMCLK和MCLK时钟源配置为XT2    4M 調試OK **************************************/
  /***************************************************************************************************
  
   P5SEL |= BIT2|BIT3; //将IO配置为XT2功能  
   UCSCTL6 &= ~XT2OFF; //使能XT2  
  
   UCSCTL4 = UCSCTL4&(~(SELA_7))|SELA_1; //先将ACLK配置为VLOCLK  
   UCSCTL3 |= SELREF_2;                  //将REFCLK配置为REFCLK  
  
    while (SFRIFG1 & OFIFG){  
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);         // 清除三类时钟标志位  
                              // 这里需要清除三种标志位，因为任何一种  
                              // 标志位都会将OFIFG置位  
    SFRIFG1 &= ~OFIFG;                                  // 清除时钟错误标志位  
  }  
  UCSCTL4 = UCSCTL4&(~(SELS_7|SELM_7))|SELS_5|SELM_5;     //将SMCLK和MCLK时钟源配置为XT2 */
  
  /****************************************************************************************************/
  /***************配置 MCLK = DCOC,SMCLK =DCOC,ACLK=XT1      25M   調試OK *****************************/
  /****************************************************************************************************/
  P5SEL |= BIT4|BIT5;  
  UCSCTL6 |= XCAP_3;  
  UCSCTL6 &= ~XT1OFF;  
  
  SetVCoreUp(1); //一次提高Vcore电压等级，具体请参考手册  
  SetVCoreUp(2);  
  SetVCoreUp(3);  
  
  __bis_SR_register(SCG0);  
  UCSCTL0 = 0;  
  UCSCTL1 = DCORSEL_6;  
  UCSCTL2 = FLLD_1 | 380;  
  __bic_SR_register(SCG0);  
  __delay_cycles(782000); 
  
 
  while (SFRIFG1 & OFIFG) {                               // Check OFIFG fault flag  
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);         // Clear OSC flaut Flags  
    SFRIFG1 &= ~OFIFG;                                  // Clear OFIFG fault flag  
  }  
  
  UCSCTL4 = UCSCTL4&(~(SELS_7|SELM_7))|SELS_3|SELM_3;
  
}
