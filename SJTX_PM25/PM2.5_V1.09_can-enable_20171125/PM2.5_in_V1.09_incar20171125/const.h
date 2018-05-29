#ifndef _const_h
#define _const_h
/*
typedef unsigned char INT8U;
typedef unsigned int INT16U;
typedef unsigned long INT32U;
*/

#define  INT8U     unsigned char
#define  INT16U    unsigned int
#define  INT32U    unsigned long

#define Nop()     {_asm nop _endasm}
#define ClrWdt()  {_asm clrwdt _endasm}
#define Sleep()   {_asm sleep _endasm}
#define Reset()   {_asm reset _endasm}



//#define     C_TIMER_1MS            0xF8F0     //  65352   // 
#define     C_TIMER_WATCHDOG           1

#define     C_SYSTEM_TIMER_INTERVAL    10

#endif  
