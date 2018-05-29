#ifndef __LIN_H
#define __LIN_H
#include	<p18cxxx.h>
//#include "mydefine.h"
//#include "Logic_handle.h"

/* 主从设置 */
#define  INT8U     unsigned char
#define  INT16U    unsigned int
#define  INT32U    unsigned long

#define TRUE    1
#define FALSE   0

#define BIT(A,B)      ((A>>B)&0x01)   /*A 为变量 */ 
#define SystemBusClock  8000000

#define AC_LIN_ID       0x20    
#define HVAC_LIN_ID     0x10    


/* 外部变量 */
//extern _SetValue    SetValue;       
//extern _SetValue    LinSetValue;    
//extern _LedSta LedCurSta;          



/*LIN协议发送结构体*/
typedef struct
{
    INT8U ID;
    INT8U Data[8];
    INT8U length ;
}LIN_Msg;

/*定义LIN状态*/ 
typedef enum
{
    IDLE, 
    _BREAK, 
    SYNCH, 
    PROTECTED_IDENTIFIER, 
    DATA0, 
    DATA1,
    DATA2, 
    DATA3, 
    DATA4, 
    DATA5,
    DATA6, 
    DATA7, 
    CHECKSUM 
}LIN_State;


// 定义帧结构体
typedef struct
{
  INT8U PID ;
  INT8U Data[8];
  INT8U Check;
  LIN_State State;
  INT8U Error;
}LIN_Frame;

/* 功能函数 */
//void LIN_Init(US baud);
INT8U LIN_GetChar(LIN_Frame *Frame_Receive);
INT8U LIN_SendBreak(void);
INT8U LIN_SendChar(INT8U ucCh);
INT8U LIN_CalcParity(INT8U id);
INT8U LINSend_CalcChecksum(INT8U id,INT8U *data,INT8U length);
INT8U LINRec_CalecChecksum( INT8U id, INT8U *data, INT8U length,INT16U checksum_send);

INT8U LINSlave_Msg_Handle(void);
void LinFillSendData(void);
void LinResolveRevData(void);
INT8U LINSlave_Send_Msg(LIN_Msg *SendMsg);

INT8U LINMaster_Send_Msg(LIN_Msg *SendMsg, INT8U ucAck);







#endif
