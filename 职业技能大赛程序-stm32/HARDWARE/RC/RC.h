#ifndef _RC_H_
#define _RC_H_

#include "sys.h"

//      光照等级
#define Light_1  1
//1等        1D    49 68 
#define Light_2  2
// 2等       2D    50 68  
#define Light_3  3
//3等        3D    51 68 
#define Light_4  4
//4等        4D    52 68 
#define ZhaDa  6
//闸道       TQ    84 81 
#define Alert_led  7
// 警灯      BJ    66 68 
#define Light_off  8


void NRF24L01_FindMyself(void);
void NRF24L01_transmit(u8 cmd);
#endif
