#ifndef _full_h
#define _full_h
#include "stm32f4xx.h"
u8   go_track( u8 speed );
void go(u8 speed ,u16 mp_value);
void back(u8 speed ,u16 mp_value);
void go_readcard(void);
void turn_left(u8 speed);
void turn_right(u8 speed);
void turn_left_delay(u16 time);
void turn_right_delay(u16 time);
u8 go_track_mp( u8 speed,u16 mp_value);
void Set_level(u8 level);
u8 mode(void);
#endif
