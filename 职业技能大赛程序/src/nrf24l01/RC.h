#ifndef _RC_H_
#define _RC_H_

#include "commom.h"

void NRF24L01_FindMyself(void);
//      ���յȼ�
#define Light_1   1
//1��        1D    49 68 
#define Light_2   2
// 2��       2D    50 68  
#define Light_3   3
//3��        3D    51 68 
#define Light_4   4
//4��        4D    52 68 
#define ZhaDao    6
//բ��       TQ    84 81 
#define Alert_led  7
// ����      BJ    66 68 
#define Light_off  8


extern uchar Repeat_count;
void NRF24L01_transmit(uchar cmd);

#endif