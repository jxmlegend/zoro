
#include<msp430f5529.h>
#include<intrinsics.h>
#include<math.h>
#include "ocmj48c.h"
#include "rencode.h"

unsigned char ADC_COMPLETE;
unsigned int results[4];
void Init_Clk(void);
unsigned long v1;
int angle;               //�Ƕ�
int Tar_angle=0,Cur_angle;//Ŀ��Ƕȣ���ǰ�Ƕ�
int  Set_angle=30;//���ýǶ�
int  wiper_angle1,wiper_angle2;
unsigned char wiper_set_num;  //�����Ƕ��л����ڱ�־
unsigned char wiper_position;
unsigned char wiper_set_ok;
unsigned char ADJUST_TIME;//����ʱ϶��־
unsigned char OK_TIME;   //�ȶ�ʱ��
unsigned char mode; //����ģʽ  0��ֻ�����Ƕȣ�����ػ�  1������㵽���趨�Ƕȣ��ȶ�5s,ͣ���յ�  2�����յ㿪ʼ���������Ƕȼ�ڶ�4�Σ�ͣ�����
typedef struct
{
  char Kp,Ki,Kd;
  int e0,e1,e2;//e0ǰ������1��ȥ��2������
  long pa,vpa;//���ڵ�ֵ������
}mypid;
mypid p1;
void PID()
{
  
    p1.e2=Tar_angle - Cur_angle;
    p1.vpa=p1.Kp*(p1.e2-p1.e1)  +p1.Ki*p1.e2  +p1.Kd*(p1.e2+p1.e0-2*p1.e1);
    p1.vpa/=10;
    p1.e0=p1.e1;
    p1.e1=p1.e2;    
    p1.pa+=p1.vpa;   
   
}
void Action()
{
  int m;
  if(p1.pa>150) p1.pa=150;
  if(p1.pa<30)  p1.pa=30;
  m=(int)(p1.pa/2.0*12);
  //if(m>1000) m=1000;
  //if(m<0)    m=0;
  TA2CCR1=m;TA2CCR2=m;
}
void SetVcoreUp (unsigned int level)  
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
/*A3��A4��A5��A6  ���е���ת�����ο���ѹ��vcc����ʵ�飬2.5v�ο�����Ч����˲���VCC��*/
void AD12_init()
{
  //P6DIR |=(BIT3+BIT4+BIT5+BIT6);
  P6SEL |=  BIT0;             //A0
  P5SEL |=  BIT0;                          //�y��VREF
  ADC12CTL2 = ADC12RES_2 + ADC12REFOUT;   // AD��������
  ADC12CTL0 |= ADC12ON + ADC12MSC + ADC12SHT0_8 ;
  
  REFCTL0|= REFVSEL_2+REFON;  //���_2.5v   REF module
  
  ADC12CTL0|= ADC12REFON + ADC12REF2_5V; 
  
  ADC12CTL1 |= ADC12SHP + ADC12CONSEQ_0;  
  
  ADC12MCTL0 |= ADC12SREF_1 + ADC12INCH_0;
      
  ADC12IE |= 0X01;                      // ʹ��ͨ��3�ж�   ��Ϊ���ת���洢�ĵ�ַ��3��ADC12MEM3����������������Ϊ00001000
}
/***********************************************************************/
/*****200��ȡƽ����������ֵv1�У���ʽ��늉���1000��ֵ*************/
/***********************************************************************/
void get_ad_result()
{
  uchar i;
  double Theta;
  int temp;
  v1=0;
  for(i=200;i>0;i--)
  {
    AD12_init();
    ADC12CTL0 |= ADC12ENC;                  // ����ת��                                                         
    ADC12CTL0 |= ADC12SC;                       // ת����ʼ
    while(!ADC_COMPLETE);
    
    ADC_COMPLETE=0;  
    v1+=results[0]; 
  }
   v1=(unsigned long) (v1*1.0/4095*12.5);
   temp=(v1-1730);
   Theta=(double)(temp/800.0);
   if(Theta>1) Theta=1;
   if(Theta<(-1)) Theta=-1;
   angle=(int)(asin(Theta) *180.0/3.1416);
   angle+=90; angle=(int)(angle*1.02);
}
//for   PWM
void TA2_init()
{
    TA2CTL|=TASSEL_2+TACLR+ID_3+MC0;  
    TA2CCR0=1000; 
    TA2CCR1=500;
    TA2CCR2=500;
    TA2CCTL1|= OUTMOD_3;
    TA2CCTL2|= OUTMOD_7;
    P2SEL |=BIT4;P2DIR |=BIT4;
    P2SEL |=BIT5;P2DIR |=BIT5;
}

//for   PID  inteval
void TA1_init()
{
    TA1CTL|=TASSEL_2+TACLR+ID_3+TAIE;  
    TA1CCR0=20000; 
}
 void main(void)
{    
      unsigned char disp_buff[]="000 ��";
      //unsigned char disp_buff2[]="0.000v";
      unsigned char disp_buff3[]="000 ��";
      unsigned char  wiper_set1[]="000 ��";
      unsigned char  wiper_set2[]="000 ��";
      unsigned char adj_flag[]="< ";unsigned char adj_flag_blank[]=" ";
      unsigned char key_count;
      unsigned char Fama_exist;
      int error_temp;
      WDTCTL = WDTPW + WDTHOLD;                       // ֹͣ���Ź�
      
      p1.Kp=12;       //pid��������
      p1.Ki=2;
      p1.Kd=3;  
      
      P6DIR |=BIT6;  //����
      
      P4DIR|=BIT1+BIT2;
      P4OUT &=~(BIT1+BIT2);
      
      Init_Clk();
      lcd_init();
      //�t�⾀��a��ʼ��
      inter_init();
      TA0_init();
      //pwm��ʼ��
      TA2_init();
      //PID �{���g��
      TA1_init();
      _EINT();// �����ж�
      
      
      while(1)
      {
          
          if(mode==0)
          {
            /*TA2CTL&=~MC0;  //�ر�PWM��ʱ��
            P2SEL &=~(BIT4+BIT5);P2DIR |=(BIT4+BIT5);P2OUT |=(BIT4+BIT5);
            lcd_init();
            disp_chars(1,0,"������װ��");
            disp_chars(0,1,"ģʽ0:�ǶȲ���");
            disp_chars(0,2,"���Ƕȣ�");
            do
            {
              get_ad_result();        
              disp_buff[0]=angle/100%10+0x30;
              disp_buff[1]=angle/10%10+0x30;
              disp_buff[2]=angle%10+0x30;
              
              disp_chars(5,2,disp_buff); 
              
              if(key!=0xff)
              {
                key_decoding(key);key=0xff;
              }
            }while(mode==0);*/
            lcd_init();
            disp_chars(0,0,"ģʽ0:PID ��ʾ");
            disp_chars(0,2,"���Ƕȣ�");
            disp_chars(0,3,"�趨�Ƕȣ�");
            TA2_init();
            TA2CCR1=100;
            TA2CCR2=500;
            Set_angle=30;key_count=0;Fama_exist=0;
            P4OUT&=~(BIT1+BIT2);
             /*�л�ģʽʱĬ��Ϊû������*/
               
            disp_chars(7,0,"  ");
            p1.Kp=12;       //pid��������1  ������
            p1.Ki=2;
            p1.Kd=3;
            TA1CCR0=20000;
            
            disp_buff3[0]=Set_angle/100+0x30;disp_buff3[1]=Set_angle/10%10+0x30;disp_buff3[2]=Set_angle%10+0x30;
            do
            {
              if(key!=0xff)
              {
                key_decoding(key);           
                if(function_key=='E')
                {
                  key_count=0;
                  Set_angle=(disp_buff3[0]-0X30)*100+(disp_buff3[1]-0X30)*10+(disp_buff3[2]-0X30);
                  ADJUST_TIME=0;TA1CTL|=MC0;  //����PID ���ƕr϶
                }
                if(function_key=='X') 
                {
                  key_count=0;
                  disp_buff3[0]=Set_angle/100+0x30;disp_buff3[1]=Set_angle/10%10+0x30;disp_buff3[2]=Set_angle%10+0x30;
                }            
                if((function_key>='0')&&(function_key<='9'))
                {
                  unsigned char j; 
                  if(key_count==3) key_count=0;
                  disp_buff3[key_count++]=function_key;
                  for(j=key_count;j<3;j++) disp_buff3[j]=' ';
                  
                }      
                if(function_key=='F')  
                {
                  Fama_exist=~Fama_exist;
                   if(Fama_exist)
                    {
                        disp_chars(7,0," F");
                        p1.Kp=6;       //pid��������2 ������
                        p1.Ki=2;
                        p1.Kd=3;
                        TA1CCR0=40000;
                    }
                    else
                    {
                        disp_chars(7,0,"  ");
                        p1.Kp=12;       //pid��������1  ������
                        p1.Ki=2;
                        p1.Kd=3;
                        TA1CCR0=20000;
                    }
                  
                }
                disp_chars(5,3,disp_buff3);
                key=0xff;
              }
              /*********************************************/
             
              /*********************************************/
              get_ad_result();        
              disp_buff[0]=angle/100%10+0x30;
              disp_buff[1]=angle/10%10+0x30;
              disp_buff[2]=angle%10+0x30;
              
              disp_chars(5,2,disp_buff);              
              disp_chars(5,3,disp_buff3);
              /************************************************/
              if(ADJUST_TIME==1)
              {
                ADJUST_TIME=0;
                get_ad_result();        
                disp_buff[0]=angle/100%10+0x30;
                disp_buff[1]=angle/10%10+0x30;
                disp_buff[2]=angle%10+0x30;
              
                disp_chars(5,2,disp_buff);       
            
                disp_chars(5,3,disp_buff3);
                Tar_angle=Set_angle;
                Cur_angle=angle;
                PID();
                Action(); 
                
                error_temp=abs(Set_angle-angle);
                if(error_temp<=5)
                {                  
                  P4OUT |=BIT1+BIT2;
                }     
                else
                {
                  P4OUT&=~(BIT1+BIT2);
                }
              }               
            }while(mode==0);
            
          }
          if(mode==1)
          {
            lcd_init();
            disp_chars(0,0,"ģʽ1:�趨ͣ��");
            disp_chars(0,2,"���Ƕȣ�");
            disp_chars(0,3,"�趨�Ƕȣ�");
            TA2_init();
            TA2CCR1=100;
            TA2CCR2=500;
            Set_angle=30;key_count=0;Fama_exist=0;
            disp_buff3[0]=Set_angle/100+0x30;disp_buff3[1]=Set_angle/10%10+0x30;disp_buff3[2]=Set_angle%10+0x30;
            ADJUST_TIME=0;TA1CTL&=~MC0;  
            P4OUT&=~(BIT1+BIT2);
             /*�л�ģʽʱĬ��Ϊû������*/
               
            disp_chars(7,0,"  ");
            p1.Kp=12;       //pid��������1  ������
            p1.Ki=2;
            p1.Kd=3;
            TA1CCR0=20000;
            
            do
            {
              if(key!=0xff)
              {
                key_decoding(key);           
                if(function_key=='E')
                {
                  key_count=0;
                  Set_angle=(disp_buff3[0]-0X30)*100+(disp_buff3[1]-0X30)*10+(disp_buff3[2]-0X30);
                  ADJUST_TIME=0;TA1CTL|=MC0;  //����PID ���ƕr϶
                }
                if(function_key=='X') 
                {
                  key_count=0;
                  disp_buff3[0]=Set_angle/100+0x30;disp_buff3[1]=Set_angle/10%10+0x30;disp_buff3[2]=Set_angle%10+0x30;
                }            
                if((function_key>='0')&&(function_key<='9'))
                {
                  unsigned char j;
                  
                  TA2CCR1=100;    //�����ּ�����ʱ���ػ���㣬���ж����ʾ
                  TA2CCR2=500;
                  Set_angle=30;
                  
                  if(key_count==3) key_count=0;
                  disp_buff3[key_count++]=function_key;
                  for(j=key_count;j<3;j++) disp_buff3[j]=' ';
                  
                }      
                if(function_key=='F')  
                {
                  Fama_exist=~Fama_exist;
                  if(Fama_exist)
                  {
                      disp_chars(7,0," F");
                      p1.Kp=6;       //pid��������2 ������
                      p1.Ki=2;
                      p1.Kd=3;
                      TA1CCR0=40000;
                  }
                  else
                  {
                      disp_chars(7,0,"  ");
                      p1.Kp=12;       //pid��������1  ������
                      p1.Ki=2;
                      p1.Kd=3;
                      TA1CCR0=20000;
                  }
                }
                disp_chars(5,3,disp_buff3);
                key=0xff;
              }
              /*********************************************/
             
              
              /*********************************************/
              get_ad_result();        
              disp_buff[0]=angle/100%10+0x30;
              disp_buff[1]=angle/10%10+0x30;
              disp_buff[2]=angle%10+0x30;
              
              disp_chars(5,2,disp_buff);              
              disp_chars(5,3,disp_buff3);
              /************************************************/
              if(ADJUST_TIME==1)
              {
                ADJUST_TIME=0;
                get_ad_result();        
                disp_buff[0]=angle/100%10+0x30;
                disp_buff[1]=angle/10%10+0x30;
                disp_buff[2]=angle%10+0x30;
              
                disp_chars(5,2,disp_buff);       
            
                disp_chars(5,3,disp_buff3);
                Tar_angle=Set_angle;
                Cur_angle=angle;
                PID();
                Action(); 
                
                error_temp=abs(Set_angle-angle);
                if(error_temp<=5)
                {                  
                  OK_TIME++;
                }
                if(OK_TIME>6)
                {
                  P4OUT |=BIT1+BIT2;
                }
              } 
              
              if(((Fama_exist)&&(OK_TIME==23))||((!Fama_exist)&&(OK_TIME==45))) //�ȶ�ʱ��ﵽҪ��ȥ�յ㣬ֹͣPID����
              {
                P4OUT &=~(BIT1+BIT2);
                TA2CCR1=500;
                TA2CCR2=900;
                ADJUST_TIME=0;
                TA1CTL&=~MC0;
                OK_TIME=0;
              }
            
            }while(mode==1);
          
          }
          if(mode==2)
          {
            lcd_init();
            disp_chars(0,0,"ģʽ2:�ڶ�����");
            disp_chars(0,1,"���Ƕȣ�");
            disp_chars(0,2,"�Ƕ�1:");
            disp_chars(0,3,"�Ƕ�2:");
            TA2_init();
            
            TA2CCR1=500;  //ͣ���յ�
            TA2CCR2=900;
            ADJUST_TIME=0;//������
            TA1CTL&=~MC0;
            key_count=0;wiper_position=0;wiper_set_num=0;Fama_exist=0;
            P4OUT&=~(BIT1+BIT2);
            wiper_set1[0]='X';wiper_set1[1]='X';wiper_set1[2]='X';
            wiper_set2[0]='X';wiper_set2[1]='X';wiper_set2[2]='X';
            /*�л�ģʽʱĬ��Ϊû������*/
               
            disp_chars(7,0,"  ");
            p1.Kp=3;       //pid��������1  ������
            p1.Ki=3;
            p1.Kd=9;
            TA1CCR0=60000;
                 
            do
            {
              if(key!=0xff)
              {
                key_decoding(key);           
                if(function_key=='E')
                {
                  key_count=0;
                  if(!wiper_set_num)
                  {
                    wiper_angle1=(wiper_set1[0]-0X30)*100+(wiper_set1[1]-0X30)*10+(wiper_set1[2]-0X30);if(wiper_angle1>130)  wiper_angle1=131;
                  }
                  else
                  {
                    wiper_angle2=(wiper_set2[0]-0X30)*100+(wiper_set2[1]-0X30)*10+(wiper_set2[2]-0X30);if(wiper_angle2>130)  wiper_angle2=131;
                    ADJUST_TIME=0;TA1CTL|=MC0;  //����PID ���ƕr϶
                    wiper_set_ok=1;
                  }
                  
                }
                if(function_key=='X') 
                {
                  key_count=0;
                  wiper_angle1=(wiper_set1[0]-0X30)*100+(wiper_set1[1]-0X30)*10+(wiper_set1[2]-0X30);
                  wiper_angle2=(wiper_set2[0]-0X30)*100+(wiper_set2[1]-0X30)*10+(wiper_set2[2]-0X30);
                  
                }            
                if((function_key>='0')&&(function_key<='9'))
                {
                  unsigned char j;
                  if(!wiper_set_num)                  
                  {
                    if(key_count==3) key_count=0;
                    wiper_set1[key_count++]=function_key;
                    for(j=key_count;j<3;j++) wiper_set1[j]=' ';
                  }
                  else
                  {
                    if(key_count==3) key_count=0;
                    wiper_set2[key_count++]=function_key;
                    for(j=key_count;j<3;j++) wiper_set2[j]=' ';
                  }                  
                }        
                if(function_key=='+') wiper_set_num=~wiper_set_num;
                if(function_key=='F')  
                {
                  Fama_exist=~Fama_exist;
                  if(Fama_exist)
                  {
                      disp_chars(7,0," F");
                      p1.Kp=4;       //pid��������1  ������
                      p1.Ki=2;
                      p1.Kd=3;
                      TA1CCR0=60000;
                  }
                  else
                  {
                      disp_chars(7,0,"  ");
                      p1.Kp=4;       //pid��������1  ������
                      p1.Ki=3;
                      p1.Kd=2;
                      TA1CCR0=60000;
                  }
                }
                key=0xff;
              }
              if(!wiper_set_num) {disp_chars(7,2,adj_flag);disp_chars(7,3,adj_flag_blank);}
              else {disp_chars(7,3,adj_flag);disp_chars(7,2,adj_flag_blank);}
               /*********************************************/           
              
              /*********************************************/
              get_ad_result();        
              disp_buff[0]=angle/100%10+0x30;
              disp_buff[1]=angle/10%10+0x30;
              disp_buff[2]=angle%10+0x30;
              disp_chars(5,1,disp_buff); 
              disp_chars(4,2,wiper_set1); 
              disp_chars(4,3,wiper_set2); 
              
              /************************************************/
              if(wiper_set_ok)
              { 
                   unsigned char k; unsigned char num;
                   wiper_set_num=0;
                   wiper_set_ok=0; wiper_position=0;
                   if(Fama_exist) num=5;
                   else num=4;
                   for(k=0;k<num;k++) //��3��
                   {
                     if(!wiper_position)Tar_angle=wiper_angle1; 
                     else 
                       Tar_angle=wiper_angle2;
                     do{
                         if(ADJUST_TIME==1)
                          {   
                            ADJUST_TIME=0;
                            get_ad_result();        
                            disp_buff[0]=angle/100%10+0x30;
                            disp_buff[1]=angle/10%10+0x30;
                            disp_buff[2]=angle%10+0x30;                          
                            disp_chars(5,1,disp_buff);  
                            Cur_angle=angle;
                            PID();
                            Action(); 
                            if(!wiper_position)  error_temp=abs(wiper_angle1-angle);
                            else error_temp=abs(wiper_angle2-angle);
                            if(error_temp<=3)
                            {                  
                              OK_TIME++;
                            }
                            if(OK_TIME>0)
                            {
                              P4OUT |=BIT1+BIT2;                              
                            }
                          } 
                          disp_chars(5,1,disp_buff); 
                           
                          if(OK_TIME==1) //�ȶ�ʱ��ﵽҪ��ȥ�յ㣬ֹͣPID����
                          {
                              OK_TIME=0;
                              P4OUT &=~(BIT1+BIT2);
                              wiper_position=~wiper_position;
                          }
                        }while(((!(k%2))&&(!wiper_position))||((k%2)&&(wiper_position)));  
                  }
                //�ڶ���ɣ������
                TA2CCR1=100;
                TA2CCR2=500;
                ADJUST_TIME=0;
                TA1CTL&=~MC0;
                OK_TIME=0;                  
              }
              
            }while(mode==2);            
          
          }                      
            
      }
}


/*************************************************
  ����(ģ��)����:void Init_Clk(void)
  ����:	ʱ�ӳ�ʼ��
  ������(ģ��)���õĺ���(ģ��)�嵥:
  ���ñ�����(ģ��)�ĺ���(ģ��)�嵥:
  �������:     void        	              		
  �������:     void       	
  ��������ֵ˵��:  		
  ʹ�õ���Դ    		
  ����˵��:       		
*************************************************/
void Init_Clk(void)
{
  /*****************************************************************************************************/ 
  /*******���� MCLK = DCOC,SMCLK =DCOC,ACLK=XT1      8M  �{ԇOK*****************************************/
  /****************************************************************************************************/
  /*  */
     P5SEL |= BIT4|BIT5;  
     UCSCTL6 |= XCAP_3;  
     UCSCTL6 &= ~XT1OFF;   //XT1����
     
     UCSCTL3 |= SELREF_2;  //��REFCLK����ΪREFCLK (�����õĻ�Ĭ�J��XT1����������ΪREFO)
     
     UCSCTL1 = DCORSEL_4;                             // ѡ��FLL�ο�Դ��REFO  
     UCSCTL2 |= 0x79;                                 // Set DCO Multiplier for 8MHz
     UCSCTL4 = SELM_3 + SELA_0 + SELS_3;              // ���� MCLK = DCOC,SMCLK =DCOC,ACLK=XT1
     UCSCTL5|= DIVS_3;                                // SMCLK ��DCO 8���l������Ϊ1MHz
     while (SFRIFG1 & OFIFG)                          //���OFIFG,and  XT1OFFG ,DCOFFG
     {
          UCSCTL7 &= ~(  XT1LFOFFG + DCOFFG);
          SFRIFG1 &= ~OFIFG;
     }  
  
}
/*************************************************************************
** �������ƣ�ADת���жϺ��� 
** ����:     ����ת��ֵ
** ����ļ�:
** �޸���־��
*************************************************************************/
#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void)
{  
  results[0] = ADC12MEM0; 
  
  ADC_COMPLETE=1;
}


//�{���r϶�r�g����
#pragma vector=TIMER1_A1_VECTOR

__interrupt void time_A1_isr()
{
      switch(TA1IV)
      {
      case 2:break;
      case 4:break;
      case 14:ADJUST_TIME=1;P6OUT ^=BIT6; break;//
      }   
  
}