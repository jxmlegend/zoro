/* ****************************************************************
** ��������: CAN�Բ����շ���̣�CAN��������0��һ����ʱ�����������ݣ����յ����ݺ�,���������ݼ�1��Ϊ��һ�εķ�������
*************************************************************** */
#include "CAN.h"
					
#define	canRX_Adress 0x05fb /* CAN���ձ�ʶ�� */
#define	canTX_Adress 0x05e2	/* CAN���ͱ�ʶ�� */
char	CAN_Adress_H,CAN_Adress_L;		/* CAN��ʶ���ߵ��ֽ� */
char	CAN_Adress_EH,CAN_Adress_EL;	/* CAN��չ��ʶ���ߵ��ֽ� */

void InitCAN_Init(void)
{
    CAN_Adress_H = canRX_Adress >>3;  // ID_CANRX_MSG_Pm25Out>>3;��������
    CAN_Adress_L = canRX_Adress <<5; //ID_CANRX_MSG_Pm25Out<<5;
    TRISB = (TRISB|0X80)&0Xbf;   
    CANCON = 0x80;                        //��������ģʽ
    while(!(CANSTATbits.OPMODE ==0x04));  //ȷ��CAN��������ģʽ
    //CIOCON =  0X00;                       //CAN I/O�ڿ��ƼĴ���������PLLʱ��

        ECANCON= 0x00;    //��ͳģʽ ,����ʣ�� 4 �����ջ�����ʱ���� FIFO �ж�,���չ����� 0�� 1 �� 2 �Լ� BRGCON2 �� BRGCON3                        
        CIOCON=0x21;      //ʹ��������Ϊ CAN ϵͳʱ��Դ ,����ʱ CANTX ���Ž����� VDD                                   
   
	    BRGCON1=0X00;	 //TQ = (2 x 1)/FOSC,00 = ͬ����ת���ʱ�� = 1 x TQ		  	          
        BRGCON2=0x98;    //0X90;	 �����ɱ��	��λ����� 1 ʱ�� = 4 x TQ ����ʱ�� = 1 x TQ	 	        
        BRGCON3=0x41;    //0X42;ʹ�� CAN ������·�˲��������� ,��λ����� 2 ʱ�� = 2 x TQ
        		             
        BSEL0=0x00;         //������ 5 �������� 0 ����ʹ��λ,ȫ��ʹ��               
        RXB0CON = 0x20;    //0B00100000;  �����մ��б�׼��ʶ������Ч����            
        RXB1CON = 0x00;    //���� RXFnSIDL �Ĵ����е� EXIDEN λ����������Ч����       

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
     TXB0CON  =  TXB0CON & 0xF7;          //���ͻ���Ĵ���
     TXB0SIDH =  canTX_Adress>>3;                    //���ͻ�������׼��ʶ��
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //���ͻ��������ݳ���
     TXB0D0   =  val1;                    //���ͻ�����0�����ֶ��ֽ�0
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
     
//    TXB0D0=RXB0D0+1;  // �ý������ݼ�1�����·������� */
    TXB0D1=val2;
    TXB0D2=val3;
    TXB0D3=val4;
    TXB0D4=0x00;
    TXB0D5=0x00;
    TXB0D6=0x00;
    TXB0D7=0x00; 
//     TXB0CON  =  TXB0CON | 0x08;          //�����ͱ���
	TXB0CONbits.TXREQ = 1;     // �������͡�
	while(TXB0CONbits.TXREQ);   // ��������û�б������ڷ��ͣ��У��ȴ���
}

void CAN_Transmit2(unsigned int Pm10_Value,unsigned int Pm25_Value,unsigned char num)
//void CAN_Transmit2(INT8U val1,INT8U val2,INT8U val3,INT8U val4,INT8U val5)//PM2.5(1-2) SN��(3) PM10(4-5)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //���ͻ���Ĵ���
     TXB0SIDH =  canTX_Adress>>3;                    //���ͻ�������׼��ʶ��
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //���ͻ��������ݳ���
//     TXB0D0   =  (val1<<6)|(val2>>2);                    //���ͻ�����0�����ֶ��ֽ�0
//    
//    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
//     
////    TXB0D0=RXB0D0+1;  // �ý������ݼ�1�����·������� */
//    TXB0D1=(val2<<6)|((val3<<1)&0x3e)|((val4>>1)&0x01);
//    TXB0D2=val4<<7|val5>>1;
//    TXB0D3=val5<<7;
	 TXB0D0   =  0xFF;                    //���ͻ�����0�����ֶ��ֽ�0
    
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
//     TXB0CON  =  TXB0CON | 0x08;          //�����ͱ���
	TXB0CONbits.TXREQ = 1;     // �������͡�
	while(TXB0CONbits.TXREQ);   // ��������û�б������ڷ��ͣ��У��ȴ���
}
void CAN_Transmit3(INT8U val1,INT8U val2,INT8U val3,INT8U val4,INT8U val5)
{   
     TXB0CON  =  TXB0CON & 0xF7;          //���ͻ���Ĵ���
     TXB0SIDH =  canTX_Adress>>3;                    //���ͻ�������׼��ʶ��
     TXB0SIDL =  (canTX_Adress<<5)%256;         
     TXB0DLC  =  0x08;                    //���ͻ��������ݳ���
     TXB0D0   =  val1<<6 &0xc0; //���ͻ�����0�����ֶ��ֽ�0
     TXB0D0  |=  val2>>2 &0x3f;                
    
    // num = CANR_Pm25OutSn;     // for Receive the PM25_OUT TEST
     
//    TXB0D0=RXB0D0+1;  // �ý������ݼ�1�����·������� */
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
//     TXB0CON  =  TXB0CON | 0x08;          //�����ͱ���
	TXB0CONbits.TXREQ = 1;     // �������͡�
	while(TXB0CONbits.TXREQ);   // ��������û�б������ڷ��ͣ��У��ȴ���
}