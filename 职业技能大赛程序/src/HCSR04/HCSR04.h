#ifndef _HCSR04_H_
#define _HCSR04_H_


#include "commom.h"
#include "delay.h"

sbit Trig =P3^7; 
sbit Echo =P4^6; 


float distance(void);


#endif
