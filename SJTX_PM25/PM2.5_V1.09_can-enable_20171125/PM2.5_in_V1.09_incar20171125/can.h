#ifndef _can_h
#define _can_h

#include    "const.h"
#include    "configure.h"	
#include    "io.h"



//FOR RX


#define     ID_CANRX_MSG_Pm25Out         0x5fc     //2017.8.11  FOR Pm25Out AC TEST Tooling;
#define     EX_CANRX_MSG_Pm25Out         0  

extern unsigned char CANR_FLAG;
INT16U     CANR_Pm25OutValue;
INT8U      CANR_Pm25OutSn;
INT8U      CANR_Pm25OutError;

INT8U CANR_SIDH;
INT8U CANR_SIDL;

extern unsigned int PM25_Value;
extern unsigned char CANR_OUT_OK;


typedef  struct
{
 INT8U     Status;

 INT8U     InitCount;
 INT8U     InitTimeoutCnt;

 INT8U     BusOff;
 INT16U    BusOffCount;
 
 
 INT8U     Pm25OutStatus;   //for Pm25Out pannel test tooling;
  
   
 
 INT16U    Pm25OutTimeoutCnt;  //for Pm25Out Pannel testt tooling;

 INT16U    ACSendTimeroutcnt1;  
 INT16U    ACSendTimeroutcnt2;     
          
}CAN_DRIVER;



typedef struct 
{
	INT32U    ID;
	INT16U    Extend;	
	INT16U    Length;
	INT8U     RxData[8];

}CAN_RXMSG;




#define     C_CAN_RXMSG_MAXLEN        30

#define     C_ECU_OK                  1
#define     C_ECU_NG                  2
#define     C_Pm25Out_OK                 1
#define     C_Pm25Out_NG                 2

#define     C_CAN_STA_OFF            0X00
#define     C_CAN_STA_INIT0          0X01
#define     C_CAN_STA_INIT1          0X02
#define     C_CAN_STA_INIT2          0X03
#define     C_CAN_STA_INIT3          0X04
#define     C_CAN_STA_INIT4          0X05
#define     C_CAN_STA_INIT5          0X06

#define     C_CAN_STA_READY          0X10
#define     C_CAN_STA_BUSOFF         0X20
#define     C_CAN_STA_HARDERROR      0XF0


#define     C_TIMER_Pm25OutTIMEROUT     600   // for Pm25Out pannel test tooling;
 
#define     C_CAN_BUSOFFTIMEOUT      200

#define     C_CAN_INITCOUNT          10
#define     C_CAN_INITTIMEOUT        50

void        Init_MCU_CAN(void);
void        CAN_Init(void);
void        CAN_Init_RxMsg(void);
INT16U      CAN_Get_RxMsg(void);
void        CAN_Put_RxMsg(void);

//INT16U      CAN_SendFrame(CAN_TXMSG msg);

void        CAN_pro(void);

INT16U      CAN_GetValue(INT8U Channel);
INT8U       CAN_GetSatus(INT8U  Channel);

#define     Pm25_Out_Sensor             14

#define     CAN_MSG_FAULT_NORMAL          1
#define     CAN_MSG_FAULT_RESOUREERROR    2 
#define     CAN_MSG_FAULT_LOSTCOMM        3
#define     CAN_MSG_FAULT_HARDERROR       4


void       PutMsg_Diag_Pm25OutComStatus( INT8U Sts);  // for Pm25Out pannel

#endif 