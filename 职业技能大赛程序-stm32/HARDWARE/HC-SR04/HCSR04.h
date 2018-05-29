#ifndef __HCSR04_H
#define __HCSR04_H 			   
#include "sys.h"
 
void HCSR04_init(void);
void EXTI9_5_IRQHandler(void);
void get_distance(void);
void EXTIDISABLE(void);
void EXTIENABLE(void);

#endif
