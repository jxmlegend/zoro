#ifndef   _input_h
#define   _input_h
#include  "const.h"
#include  "configure.h"	

INT16U Get_AD_Word(INT8U);

void  Input_Sensor_Proc(void);
void  Init_MCU_AD(void);


void IGN_Feedback_Init(void);
void IGN_Feedback_Detect(void);
INT16U  IGN_Feedback_AdtoVoltage(INT16U);

void BATT_Feedback_Init(void);
void BATT_Feedback_Detect(void);
INT16U  BATT_Feedback_AdtoVoltage(INT16U);


/*Interfer function for application layer
***********************************************************************/

#define    IGN_FEEDBACK               3
#define    BATT_FEEDBACK              4 


INT16U     Feedback_GetValue( INT8U Channel);

INT8U      Feedback_GetSatus(INT8U Channel);


/**********************************************************************/

#define    C_FAULT_NONE                      0X01
#define    C_FAULT_SHORTTOGND                0X02
#define    C_FAULT_SHORTTOBATT               0X03
#define    C_FAULT_OPEN                      0X04

#define    C_IOTHREHOLD_SHORTTOGND           10
#define    C_IOTHREHOLD_SHORTTOBATT          1014
 
#define    C_MODULE_SHORTTOGND                4
#define    C_MODULE_SHORTTOBATT              182

#define    C_TIMER_FAULTDETECT               1000        
#define    C_AD_MAX_NUM                       6            //how many ad channel   Org: 6  
  

#define    C_SMOOTHING_COUNT                 30


#define     C_IGN_NORMAL                      0X01
#define     C_IGN_UNDERVOLTAGE                0X02
#define     C_IGN_0VERVOLTAGE                 0X03

#define     C_BATT_NORMAL                      0X01
#define     C_BATT_UNDERVOLTAGE                0X02
#define     C_BATT_0VERVOLTAGE                 0X03

//#define     C_TIMER_OVERVOLTAGE                 500
//#define     C_TIMER_OVERVOLTAGERETURN           1000
//#define     C_TIMER_UNDERVOLTAGE                500
//#define     C_TIMER_UNDERVOLTAGERETURN          1000

#define     C_TI_IGN_O_V                         400          //ORG: 500
#define     C_TI_IGN_O_V_R                       800         //ORG:1000
#define     C_TI_IGN_U_V                         400          //ORG:500
#define     C_TI_IGN_U_V_R                       800         //ORG:1000

#define     C_TI_BATT_O_V                         400
#define     C_TI_BATT_O_V_R                       800
#define     C_TI_BATT_U_V                         400
#define     C_TI_BATT_U_V_R                       800


//#define     C_VOLTHREHOLD_OVERVOLTAGE           840   //16500
//#define     C_VOLTHREHOLD_OVERVOLTAGERETURN     813   //16000   org:863
//#define     C_VOLTHREHOLD_UNDERVOLTAGE          427   //8500   ORG: 407
//#define     C_VOLTHREHOLD_UNDERVOLTAGERETURN    434   //9000
 

#define     C_V_TH_IGN_O_V                      788  //690      //ORG  840
#define     C_V_TH_IGN_O_V_R                    766  //675      //ORG   813
#define     C_V_TH_IGN_U_V                      251  //365
#define     C_V_TH_IGN_U_V_R                    272  //380

#define     C_V_TH_BATT_O_V                      788  //690
#define     C_V_TH_BATT_O_V_R                    766  //675
#define     C_V_TH_BATT_U_V                      251  //365
#define     C_V_TH_BATT_U_V_R                    272  //380

typedef struct
{
 INT8U   Init;
 INT8U   Status;
     
 INT16U   Value_ECU;
 INT16U   Value_UDS;
 INT16U   Value_IN;

 INT8U    Interval;
 INT16U   TimerOutCountOverVoltage;
 INT16U   TimerOutCountUnderVoltage;
 INT16U   TimerOutCountReturnVoltage;
 
 INT32U   Value_SUM;
 INT8U    Count;
 INT16U   Buffer[C_SMOOTHING_COUNT];  

}
SUPPLY_VOLTAGE_CONTROL;




#endif