/* 
 * File:   CAN.h
 * Author: ZERO
 *
 * Created on 2017��12��21��, ����2:52
 */

#ifndef CAN_H
#define	CAN_H

#include	<p18cxxx.h>

#define  INT8U     unsigned char
#define  INT16U    unsigned int
#define  INT32U    unsigned long

void InitCAN_Init(void);
void CAN_Transmit(INT8U ,INT8U ,INT8U ,INT8U );
//void CAN_Transmit2(INT8U ,INT8U ,INT8U ,INT8U ,INT8U );
void CAN_Transmit2(unsigned int Duty,unsigned int Value,unsigned char Degree);
#endif	/* CAN_H */

