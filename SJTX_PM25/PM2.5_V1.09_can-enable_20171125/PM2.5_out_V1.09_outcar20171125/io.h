#ifndef    _io_h
#define    _io_h

#include  "const.h"
#include  "configure.h "

typedef  struct
{

  //INT16U    Init;
   INT8U      Dir;
   INT8U      Status;
   INT16U     Value_IN;
   INT16U     Value_SET;
   INT16U     Value_BAK;
   INT16U     Timer_ACT;               //must be placed in the big timer
   //   INT16U    Count;     

}
MOTOR_CONTROL;




#define    C_MOTOR_DIR_STOP         1
#define    C_MOTOR_DIR_BREAK        2
#define    C_MOTOR_DIR_UP           3
#define    C_MOTOR_DIR_DOWN         4

#define    C_PRECISION_MOTOR_ACT    2



#define	   C_TIME_ACT_BRAKE		    100    
#define	   C_TIME_ACT_LOCK	        8000  

#define    C_TIMER_ACT_REC_LOCK     8000  


/*****************INTERFACE** ********************************/
void        Set_Mode_Flap(INT8U Sts);
void        Set_Mix_Flap(INT8U Sts);
void        Set_Rec_Flap(INT8U Sts);
void        Set_AC_Con(INT8U  Sts);
void        Set_Reardef(INT8U Sts);
void        Set_Blower_Speed(INT8U Sts);



#define     FACE         0
#define     FACE_FOOT    1
#define     FOOT         2
#define     FOOT_DEF     3
#define     DEF          4


#define     INSIDE             0
#define     OUTSIDE            1

#define     FUN_ON            1
#define     FUN_OFF           0




/*
#define     LEVEL0      0
#define     LEVEL1      26// 345
#define     LEVEL2      28// 40
#define     LEVEL3      31// 45
#define     LEVEL4      33// 50
#define     LEVEL5      35// 55
#define     LEVEL6      40// 60
#define     LEVEL7      43// 65
#define     LEVEL8      48// 70
#define     LEVEL9      50// 75
#define     LEVEL10     53//  80
#define     LEVEL11     55//  85     
#define     LEVEL12     58// 90
#define     LEVEL13     65// 95
*/
//#define LEVEL14    100


#define     LEVEL0      0
#define     LEVEL1      1   //26// 345
#define     LEVEL2      2   //28// 40
#define     LEVEL3      3   //31// 45
#define     LEVEL4      4   //33// 50
#define     LEVEL5      5   //35// 55
#define     LEVEL6      6   //40// 60
#define     LEVEL7      7   //43// 65
#define     LEVEL8      8   //48// 70
#define     LEVEL9      9   //50// 75
#define     LEVEL10     10  //53// 80
#define     LEVEL11     11  //55// 85     
#define     LEVEL12     12  //58// 90
#define     LEVEL13     13  //65// 95



#define	  	LCD_BUFF_LEN    6
#define     TIMER_IIC_WAIT  1000
#define     LCD_HI          300
#define     LCD_LO          180

void Set_Lcd_Back(INT8U   Sts);
void Set_Lcd_Fullon(INT8U Sts);
void Set_Lcd_Blower(INT8U Sts);
void Set_Lcd_Mode(INT8U  Sts);
void Set_Lcd_Tem(INT16U Sts);
void Set_Lcd_Ac(INT8U Sts);
void Set_Lcd_Rec(INT8U Sts);
void Set_Lcd_Econ(INT8U Sts);
void Set_Lcd_Auto(INT8U Sts);
/****************************************************************/

void   Set_Hardware_Mode(INT8U );
void   Set_Hardware_Tem(INT8U );
void   Set_Hardware_Rec(INT8U );
void   Set_Hardware_Blw_Relay(INT8U);
void   Set_Hardware_PWM(INT16U);
void   Set_Hardware_AC(INT8U);
void   Set_Hardware_Reardef(INT8U);



void   Set_Hardware_Lcd_Back(INT8U n);
void   Set_Hardware_Lcd_Fullon(INT8U n);
void   Set_Hardware_Lcd_Blower(INT8U n);
void   Set_Hardware_Lcd_Mode(INT8U n);
void   Set_Hardware_Lcd_Tem(INT16U n);
void   Set_Hardware_Lcd_Ac(INT8U n);
void   Set_Hardware_Lcd_Rec(INT8U n);
void   Set_Hardware_Lcd_Econ(INT8U n);
void   Set_Hardware_Lcd_Auto(INT8U n);


void   Motor_Tem_Action_Pro(void);
void   Motor_Mode_Action_Pro(void);
void   Motor_Rec_Action_Pro(void);
void   NCV7708_Proc(void) ;
void   Data1_Display( INT8U Data);
void   Data2_Display( INT8U Data);
void   Data3_Display( INT8U Data);
void   Out_Lcd_HT16C21(void);
void   Lcd_Proc(void);



#define  C_PWM_MAX                   254       
#define  C_TIMER_BLW_PWM_STEP        20  
#define  C_TIMER_BLW_PWM_STEP_AUTO   50 
#define  C_TIMER_BLW_PWM_START       1000



  
void   Blower_Action_Pro(void);
void   AC_Action_Pro(void);
void   Reardef_Action_Pro(void);


/*mcu initial core
*******************************************/
void  Init_MCU_IO(void);
void  Init_MCU_Clock(void);
void  Init_MCU_Timer(void);
void  Init_MCU_AD(void);
void  Init_MCU_PWM(void);
void  Init_WatchDog(void);

/*********************************************/

void  Init_Ram(void );

/********************************************/


void  Init_LCD(void);
void  Write_Byte_Comand( INT8U Command, INT8U Data_Setting);
void  Write_Signal_Data(INT8U Add, INT8U Data);
void  Write_Page_Data(INT8U Add1, INT8U Add2, INT8U Add3);
void  IIC_Start(void);
void  IIC_ChaAck(void);
void  IIC_Send8bit(INT8U Data);
void  IIC_Stop(void);


/********************************************/
void  Init_I2c(void);
#endif   