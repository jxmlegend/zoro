/* 
 * File:   AD.h
 * Author: ZERO
 *
 * Created on 2018��2��3��, ����3:05
 */

#ifndef AD_H
#define	AD_H

#include	<p18cxxx.h>
#include	<delays.h>

#define  INT8U     unsigned char
#define  INT16U    unsigned int
#define  INT32U    unsigned long

void AD_Init(void);
INT16U AD_GET(INT8U channel);

#endif	/* AD_H */