#ifndef _COMMOM_H_
#define _COMMOM_H_


#include <STC12C5A60S2.H>
//#include <stc11f08.h>

#include <intrins.h>
#include <stdio.h>

typedef unsigned int uint16;
typedef unsigned int UINT;
typedef unsigned int uint;
typedef unsigned int UINT16;
typedef unsigned int WORD;
typedef unsigned int word;
typedef unsigned int u16;
typedef int int16;
typedef int INT16;
typedef unsigned long uint32;
typedef unsigned long UINT32;
typedef unsigned long DWORD;
typedef unsigned long dword;
typedef long int32;
typedef long INT32;
typedef signed char int8;
typedef signed char INT8;
typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned char uchar;
typedef unsigned char uchar8;
typedef unsigned char UINT8;
typedef unsigned char uint8;
typedef unsigned char BOOL;
typedef unsigned char u8;

/*1�� NRF24L01�ܽ����� */
sbit CE   = P2^0;  // Chip Enable pin signal (output)
sbit SCK  = P2^1;  // Master Out, Slave In pin (output)	 
sbit MISO = P2^2;  // Master In, Slave Out pin (input) 
sbit MOSI = P2^3;  // Serial Clock pin, (output)
sbit IRQ  = P3^2;  // Interrupt signal, from nRF24L01 (input)
sbit CSN  = P4^4;  // Slave Select pin, (output to CSN, nRF24L01)

/*2�� ѭ���˿� */
#define  xunji  P0
/*
ʵ�����ߣ�
��Դ�����������------51���ذ�
				ENB----P1.4
				IN4-----P1.5
				IN3----P1.2
				ENA----P1.3
				IN2----P1.1
				IN1----P1.0
*/
/* 3��ֱ������ܽ����� */
sbit MOTOR_A_CON1=P1^0;
sbit MOTOR_A_CON2=P1^1; 

sbit MOTOR_A_EN  =P1^3; 
sbit MOTOR_B_EN  =P1^4;

sbit MOTOR_B_CON1=P1^2;   
sbit MOTOR_B_CON2=P1^5;



/*4�����ⷢ����ƶ˿�*/
sbit control_send=P1^6;

/**5����ɫ���������Ŷ���**/  

sbit s2=P3^4;        //TCS3200 S2 
sbit s3=P3^6;        //TCS3200 S3
                     //TCS3200 S0 ģ���ڲ�Ĭ����
                     //TCS3200 S1 ģ���ڲ�Ĭ������
                     //TCS3200 OE ģ���ڲ��ӵ�
//TCS3200 OUT ��P3.5	
/*
6��zigbee���ƶ˿�
zibeeģ���P0_4������С�����ذ��P3.7���� 
*/
sbit zigbee_control=P3^7;

/*7����ʪ�ȴ������˿�*/
sbit  DHT11_OUT    =P2^5;	

/*8��������*/
sbit BEEP=P1^7;

/*9�����մ�����BH1750FVI IO����*****/
sbit    BH1750_SCL=P2^6;      //IICʱ�����Ŷ���
sbit  	BH1750_SDA=P2^7;      //IIC�������Ŷ���
 
 
#endif