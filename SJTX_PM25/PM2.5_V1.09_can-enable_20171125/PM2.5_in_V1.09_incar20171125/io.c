#include       "io.h"
#include       "input.h"
#include       "configure.h"
#include       "motorris.h"
#include       "can.h"

#define         Lcdbitset(n)		      LCD_BUFF[n/8]|=(0X01)<<(n%8)
#define         Lcdbitclr(n) 		  LCD_BUFF[n/8]&=~((0X01)<<(n%8))
extern          unsigned char     Menu_AUTO;
INT16U          FLG_7708A;
INT16U          FLG_E910;
INT16U          BAK_FLG_7708A;


INT8U           IIC_ACK_Flag;
INT16U          Timer_IIC_Waitting;


MOTOR_CONTROL     Motor_Control_Mode;  
MOTOR_CONTROL     Motor_Control_Rec;
MOTOR_CONTROL     Motor_Control_Tem;

//application layer to the bottom layer interface
INT8U            Set_Posi_Tem;
INT8U            Set_Posi_Mode;
INT8U            Set_Posi_Rec;
INT8U            Set_Blw_Level;
INT8U            Set_AC;
INT8U            Set_Reardefo;

INT8U            Set_Lcd_Backlight;
INT8U            Set_Lcd_FullOn;

INT8U            Set_Lcd_Blw;
INT8U            Set_Lcd_Modedrive;
INT16U           Set_Lcd_Temp;
INT8U            Set_Lcd_Acdrive;
INT8U            Set_Lcd_Recdrive;
INT8U            Set_Lcd_Econdrive;
INT8U            Set_Lcd_Autodrive;


//bottom to the physcial layer

INT8U           ACT_TEM_DIR;
INT8U           ACT_MODE_DIR;
INT8U           ACT_REC_DIR;
INT8U           ACT_BLW_RELAY_OUT;
INT8U           ACT_AC_OUT;
INT8U           ACT_REARDEF_OUT;
INT8U           ACT_LCD_BACKLITHT_OUT;
////////////////////////////////////////

INT16U         Blw_PWM;
INT16U         Timer_Blw_PWM_Step;

INT8U  	  	   LCD_BUFF[LCD_BUFF_LEN];

//应用层与底层的接口

void          Set_Mode_Flap(INT8U Sts)     {Set_Posi_Mode=Sts;}
void          Set_Mix_Flap(INT8U Sts)      {Set_Posi_Tem=Sts; }
void          Set_Rec_Flap(INT8U Sts)      {Set_Posi_Rec=Sts;}
void          Set_AC_Con(INT8U Sts)        {Set_AC=Sts;}
void          Set_Reardef(INT8U Sts)       {Set_Reardefo=Sts;}
void          Set_Blower_Speed(INT8U Sts)  {Set_Blw_Level=Sts;}



void          Set_Lcd_Back(INT8U Sts)      {Set_Lcd_Backlight =Sts;}
void          Set_Lcd_Fullon(INT8U Sts)    {Set_Lcd_FullOn=Sts; }
void          Set_Lcd_Blower(INT8U Sts)    {Set_Lcd_Blw=Sts; }

void          Set_Lcd_Mode(INT8U Sts)      {Set_Lcd_Modedrive=Sts;}
void          Set_Lcd_Tem(INT16U Sts)      {Set_Lcd_Temp=Sts; }
void          Set_Lcd_Ac(INT8U Sts)        {Set_Lcd_Acdrive=Sts; }
void          Set_Lcd_Rec(INT8U Sts)       {Set_Lcd_Recdrive=Sts; }
void          Set_Lcd_Econ(INT8U Sts)      {Set_Lcd_Econdrive=Sts; }
void          Set_Lcd_Auto(INT8U Sts)      {Set_Lcd_Autodrive=Sts; }



//The interface
 
void   Set_Hardware_Mode(INT8U Sts)
{
ACT_MODE_DIR=Sts;
}

void   Set_Hardware_Tem(INT8U Sts)
{
ACT_TEM_DIR=Sts;
}

void   Set_Hardware_Rec(INT8U Sts)
{
ACT_REC_DIR=Sts;
}

void Set_Hardware_Blw_Relay(INT8U Sts)
{
ACT_BLW_RELAY_OUT=Sts;
}

void   Set_Hardware_PWM(INT16U Sts)
{ 
  CCPR3L    = Sts/4; 
  CCP3CON  =  0xFC;                    
  CCP3CON &= ((((Sts%4)<<4)&0X30)|0xCF);   //to be think
}
void Set_Hardware_AC(INT8U Sts)
{

ACT_AC_OUT=Sts;

}

void Set_Hardware_Reardef(INT8U Sts)
{
ACT_REARDEF_OUT=Sts;
}



void  Motor_Tem_Action_Pro()
{

 //if(0)   return;           //when head is off or when the voltage is error 

 Motor_Control_Tem.Value_IN=Feedback_GetValue(MIX_MOTOR_FEEDBACK);

if(Feedback_GetSatus(MIX_MOTOR_FEEDBACK)!=C_FAULT_NONE)
{
 Motor_Control_Tem.Dir=C_MOTOR_DIR_STOP;
}
else
{

 Motor_Control_Tem.Value_SET=TABLE_MOTOR_TEM_SET[Set_Posi_Tem];
 if(Motor_Control_Tem.Value_SET !=Motor_Control_Tem.Value_BAK)
 {
  Motor_Control_Tem.Dir=C_MOTOR_DIR_STOP;
  Motor_Control_Tem.Value_BAK=Motor_Control_Tem.Value_SET;

  if(Motor_Control_Tem.Value_IN > Motor_Control_Tem.Value_SET+C_PRECISION_MOTOR_ACT)
  {
   Motor_Control_Tem.Dir=C_MOTOR_DIR_UP;
   Motor_Control_Tem.Timer_ACT =C_TIME_ACT_LOCK;
  }
  else if(Motor_Control_Tem.Value_IN<Motor_Control_Tem.Value_SET-C_PRECISION_MOTOR_ACT)
  {
   Motor_Control_Tem.Dir=C_MOTOR_DIR_DOWN;
   Motor_Control_Tem.Timer_ACT =C_TIME_ACT_LOCK;
  }
 }

if(Motor_Control_Tem.Dir==C_MOTOR_DIR_UP)
{
if((Motor_Control_Tem.Value_IN<=(Motor_Control_Tem.Value_SET+C_PRECISION_MOTOR_ACT))&&(Motor_Control_Tem.Value_IN>=(Motor_Control_Tem.Value_SET-C_PRECISION_MOTOR_ACT)))
{ Motor_Control_Tem.Dir=C_MOTOR_DIR_BREAK;     Motor_Control_Tem.Timer_ACT =C_TIME_ACT_BRAKE; 
  PutMsg_Diag_MixMoveStatus(0);                 //tell can the moving is ok
}      
else if(Motor_Control_Tem.Timer_ACT==0) 
{
 Motor_Control_Tem.Dir=C_MOTOR_DIR_BREAK;      Motor_Control_Tem.Timer_ACT =C_TIME_ACT_BRAKE;   
                                               //except the condition power is error and mix feedback is error
 if(Feedback_GetSatus(MIX_MOTOR_FEEDBACK)==C_FAULT_NONE&&Feedback_GetSatus( BATT_FEEDBACK )==C_POWER_NORMAL)                           
 {PutMsg_Diag_MixMoveStatus(1); }              //tell the can there a mistake for the motor stuck
 }   
else{;}
}

if(Motor_Control_Tem.Dir==C_MOTOR_DIR_DOWN)
{
if((Motor_Control_Tem.Value_IN>=(Motor_Control_Tem.Value_SET-C_PRECISION_MOTOR_ACT))&&(Motor_Control_Tem.Value_IN<=(Motor_Control_Tem.Value_SET+C_PRECISION_MOTOR_ACT)))
{ Motor_Control_Tem.Dir=C_MOTOR_DIR_BREAK;     Motor_Control_Tem.Timer_ACT =C_TIME_ACT_BRAKE;
  PutMsg_Diag_MixMoveStatus(0);
}
else if(Motor_Control_Tem.Timer_ACT==0) 
{
 Motor_Control_Tem.Dir=C_MOTOR_DIR_BREAK;     Motor_Control_Tem.Timer_ACT =C_TIME_ACT_BRAKE;
                                                           //except the condition power is error and mix feedback is error
 if(Feedback_GetSatus(MIX_MOTOR_FEEDBACK)==C_FAULT_NONE&&Feedback_GetSatus( BATT_FEEDBACK )==C_POWER_NORMAL)                           
 {PutMsg_Diag_MixMoveStatus(1); }              //tell the can there a mistake for the motor stuck
}
else {;}
}

if(Motor_Control_Tem.Dir==C_MOTOR_DIR_BREAK)
{   ;
   if(Motor_Control_Tem.Timer_ACT==0)
     {Motor_Control_Tem.Dir=C_MOTOR_DIR_STOP;}
}

}

Set_Hardware_Tem(Motor_Control_Tem.Dir);

}


void  Motor_Mode_Action_Pro( )
{

//if (0)   return;                  //when the head is off when the power is error 

Motor_Control_Mode.Value_IN=Feedback_GetValue(MODE_MOTOR_FEEDBACK);

if(Feedback_GetSatus(MODE_MOTOR_FEEDBACK)!=C_FAULT_NONE)
{
Motor_Control_Mode.Dir=C_MOTOR_DIR_STOP;
}
else
{

 Motor_Control_Mode.Value_SET=TABLE_MOTOR_MODE_SET[Set_Posi_Mode];
 if( Motor_Control_Mode.Value_SET != Motor_Control_Mode.Value_BAK)
 {
   Motor_Control_Mode.Dir=C_MOTOR_DIR_STOP;
   Motor_Control_Mode.Value_BAK= Motor_Control_Mode.Value_SET;

  if( Motor_Control_Mode.Value_IN >  Motor_Control_Mode.Value_SET+C_PRECISION_MOTOR_ACT)
  {
    Motor_Control_Mode.Dir=C_MOTOR_DIR_DOWN;
    Motor_Control_Mode.Timer_ACT =C_TIME_ACT_LOCK;
  }
  else if( Motor_Control_Mode.Value_IN< Motor_Control_Mode.Value_SET-C_PRECISION_MOTOR_ACT)
  {
    Motor_Control_Mode.Dir=C_MOTOR_DIR_UP;
    Motor_Control_Mode.Timer_ACT =C_TIME_ACT_LOCK;
  }
 }

if( Motor_Control_Mode.Dir==C_MOTOR_DIR_DOWN)
{
if(( Motor_Control_Mode.Value_IN<=( Motor_Control_Mode.Value_SET+C_PRECISION_MOTOR_ACT))&&( Motor_Control_Mode.Value_IN>=( Motor_Control_Mode.Value_SET-C_PRECISION_MOTOR_ACT)))
{  
 Motor_Control_Mode.Dir=C_MOTOR_DIR_BREAK;      Motor_Control_Mode.Timer_ACT =C_TIME_ACT_BRAKE;
 PutMsg_Diag_ModeMoveStatus(0); 
 }
else if( Motor_Control_Mode.Timer_ACT==0) 
{
 Motor_Control_Mode.Dir=C_MOTOR_DIR_BREAK;       Motor_Control_Mode.Timer_ACT =C_TIME_ACT_BRAKE;

 if(Feedback_GetSatus(MODE_MOTOR_FEEDBACK)==C_FAULT_NONE&&Feedback_GetSatus( BATT_FEEDBACK )==C_POWER_NORMAL) 
{
 PutMsg_Diag_ModeMoveStatus(1);
} 
}

else{;}
}

if(Motor_Control_Mode.Dir==C_MOTOR_DIR_UP)
{
if(( Motor_Control_Mode.Value_IN>=( Motor_Control_Mode.Value_SET-C_PRECISION_MOTOR_ACT))&&( Motor_Control_Mode.Value_IN<=( Motor_Control_Mode.Value_SET+C_PRECISION_MOTOR_ACT)))
{ 
  Motor_Control_Mode.Dir=C_MOTOR_DIR_BREAK;      Motor_Control_Mode.Timer_ACT =C_TIME_ACT_BRAKE;
  PutMsg_Diag_ModeMoveStatus(0);
}
else if( Motor_Control_Mode.Timer_ACT==0) 
{

 Motor_Control_Mode.Dir=C_MOTOR_DIR_BREAK;      Motor_Control_Mode.Timer_ACT =C_TIME_ACT_BRAKE;
 if(Feedback_GetSatus(MODE_MOTOR_FEEDBACK)==C_FAULT_NONE&&Feedback_GetSatus( BATT_FEEDBACK )==C_POWER_NORMAL) 
 {
 PutMsg_Diag_ModeMoveStatus(1);
 }

}
else
{;}
}

if( Motor_Control_Mode.Dir==C_MOTOR_DIR_BREAK)
{  
if( Motor_Control_Mode.Timer_ACT==0)
{ Motor_Control_Mode.Dir=C_MOTOR_DIR_STOP;}
}
}

Set_Hardware_Mode(Motor_Control_Mode.Dir);
}

void  Motor_Rec_Action_Pro( )
{

Motor_Control_Rec.Value_SET=Set_Posi_Rec;
if(Motor_Control_Rec.Value_SET !=Motor_Control_Rec.Value_BAK)
{
  Motor_Control_Rec.Dir= C_MOTOR_DIR_STOP;
  Motor_Control_Rec.Value_BAK=Motor_Control_Rec.Value_SET;
  Motor_Control_Rec.Timer_ACT=C_TIMER_ACT_REC_LOCK;
  if(Motor_Control_Rec.Value_SET==INSIDE)
  {
  Motor_Control_Rec.Dir= C_MOTOR_DIR_DOWN;
  
  }
  else if (Motor_Control_Rec.Value_SET==OUTSIDE)
  {
   Motor_Control_Rec.Dir= C_MOTOR_DIR_UP;
   
  }
}

if(Motor_Control_Rec.Dir==C_MOTOR_DIR_DOWN)
{ 

if(Motor_Control_Rec.Timer_ACT==0)    { Motor_Control_Rec.Dir= C_MOTOR_DIR_BREAK; Motor_Control_Rec.Timer_ACT=C_TIME_ACT_BRAKE;}                         
}
if(Motor_Control_Rec.Dir==C_MOTOR_DIR_UP)
{

if(Motor_Control_Rec.Timer_ACT==0)    { Motor_Control_Rec.Dir= C_MOTOR_DIR_BREAK; Motor_Control_Rec.Timer_ACT=C_TIME_ACT_BRAKE;}                         
}
if(Motor_Control_Rec.Dir==C_TIME_ACT_BRAKE)
{
if(Motor_Control_Rec.Timer_ACT==0)    { Motor_Control_Rec.Dir= C_MOTOR_DIR_STOP; }
}

Set_Hardware_Rec( Motor_Control_Rec.Dir );

}


void   Blower_Action_Pro()
{
INT16U  n;
{
 n=TABLE_BLOWER_LEVEL_SET[Set_Blw_Level];

/*
if(n>0&&Blw_PWM==0)
{
   Blw_PWM=TABLE_BLOWER_LEVEL_SET[1];
   Timer_Blw_PWM_Step=C_TIMER_BLW_PWM_START;
}
*/


if(Timer_Blw_PWM_Step==0)
{
 if(n<Blw_PWM)
 {

 if(Blw_PWM>0)
 { Blw_PWM--;  }	

 }
 else if(n>Blw_PWM)
 {

  Blw_PWM++;          //Timer_Blw_PWM_Step = C_TIMER_BLW_PWM_STEP;
  if(Blw_PWM>=0X3FD)     {Blw_PWM=0X3FD;}

 }
 else  if(n==Blw_PWM)
 {
  Blw_PWM=n;    
                      //Timer_Blw_PWM_Step = C_TIMER_BLW_PWM_STEP;
 } else
 {;}


 if(Menu_AUTO)
 {Timer_Blw_PWM_Step = C_TIMER_BLW_PWM_STEP_AUTO;}
 else
 {Timer_Blw_PWM_Step = C_TIMER_BLW_PWM_STEP;}
 


if(Set_Blw_Level==0)
{
  Blw_PWM=TABLE_BLOWER_LEVEL_SET[C_BLW_MIN];
}

}


if(Set_Blw_Level==0)
{ 
 Set_Hardware_Blw_Relay(FUN_OFF);
}
else
{
 Set_Hardware_Blw_Relay(FUN_ON);
}

}

Set_Hardware_PWM(Blw_PWM); 
}


void   AC_Action_Pro()
{   
   if(Set_AC==FUN_ON)
  {  Set_Hardware_AC(FUN_ON) ;}
   else  
  {  Set_Hardware_AC (FUN_OFF);}    
}



void   Reardef_Action_Pro()
{  
    if(Set_Reardefo==FUN_ON)
    Set_Hardware_Reardef(FUN_ON);
    else
    Set_Hardware_Reardef(FUN_OFF);
 }


void   NCV7708_Proc(void )
{
INT8U   i;

if(ACT_BLW_RELAY_OUT==FUN_ON)  {OUT_MODULE_ENABLE =1; OUT_MODULE_PROTECT=0;}   else {OUT_MODULE_ENABLE=0; OUT_MODULE_PROTECT=1; } 
if(ACT_AC_OUT==FUN_ON)         {OUT_AC =1;
 PutMsg_Calibr_AcOutputStatus(1);}              
else {OUT_AC=0; PutMsg_Calibr_AcOutputStatus(0); }
if(ACT_REARDEF_OUT==FUN_ON)    {OUT_REDEF =1;}             else {OUT_REDEF =0;          } 
if(ACT_LCD_BACKLITHT_OUT==FUN_ON) { OUT_LCD_BACKLIGHT =1;} else{OUT_LCD_BACKLIGHT=0;  }

FLG_7708A    =  0x0000; 
FLG_7708A   |=  C_7708_IN_SET_OCD;                      
  
if(((BAK_FLG_7708A&0X2000)==C_7708_OUT_SET_OCD)||((BAK_FLG_7708A&0X0001)==C_7708_OUT_SET_TW))
{
FLG_7708A   |=   C_7708_IN_SET_SRR;   
}
else{FLG_7708A   &= (~C_7708_IN_SET_SRR);}


if(ACT_TEM_DIR==C_MOTOR_DIR_BREAK)
{FLG_7708A |=C_MIX_MOTOR_HA;     FLG_7708A |=C_MIX_MOTOR_HB;}
else if(ACT_TEM_DIR==C_MOTOR_DIR_UP)
{FLG_7708A |= C_MIX_MOTOR_HB;    FLG_7708A |=C_MIX_MOTOR_LA;}
else if(ACT_TEM_DIR==C_MOTOR_DIR_DOWN)
{FLG_7708A |=C_MIX_MOTOR_HA ;    FLG_7708A |=C_MIX_MOTOR_LB;}
else 
{;}

if(ACT_MODE_DIR==C_MOTOR_DIR_BREAK )
{FLG_7708A |=C_MODE_MOTOR_HA;    FLG_7708A |=C_MODE_MOTOR_HB;}
else if (ACT_MODE_DIR==C_MOTOR_DIR_UP)
{FLG_7708A |=C_MODE_MOTOR_HB;    FLG_7708A |=C_MODE_MOTOR_LA;}
else if (ACT_MODE_DIR==C_MOTOR_DIR_DOWN)
{FLG_7708A |=C_MODE_MOTOR_HA;    FLG_7708A |=C_MODE_MOTOR_LB;}
else 
{;}

if(ACT_REC_DIR==C_MOTOR_DIR_BREAK)
{FLG_7708A |=C_REC_MOTOR_HA;    FLG_7708A |=C_REC_MOTOR_HB;}
else if (ACT_REC_DIR==C_MOTOR_DIR_UP)
{FLG_7708A |=C_REC_MOTOR_HB;    FLG_7708A |=C_REC_MOTOR_LA; PutMsg_Calibr_RecPosition(1);}
else if (ACT_REC_DIR==C_MOTOR_DIR_DOWN)
{FLG_7708A |=C_REC_MOTOR_HA;    FLG_7708A |=C_REC_MOTOR_LB; PutMsg_Calibr_RecPosition(0);}
else {;}

TO_7708_ENABLE=1;
TO_7708_CSB=1;
TO_7708_CLK=0;
TO_7708_SI=0;
Nop() ;Nop() ;Nop() ;Nop() ;
TO_7708_CSB=0;
Nop() ;Nop() ;Nop() ;
for(i=0;i<16;i++)
{
 
 TO_7708_CLK=1; Nop() ;Nop() ;Nop() ;
 if(FLG_7708A&0X0001) TO_7708_SI=1; else TO_7708_SI=0;
 TO_7708_CLK=0; Nop() ;Nop() ;Nop() ;
 FLG_7708A>>=1;
 if(FROM_7708_SO=1) FLG_7708A|=0X8000;else FLG_7708A&=0X7FFF;

}
BAK_FLG_7708A=FLG_7708A; 
TO_7708_CSB=1;

}


void   Set_Hardware_Lcd_Back(INT8U n)
{
 
  ACT_LCD_BACKLITHT_OUT=n; 

}




void   Set_Hardware_Lcd_Fullon(INT8U n)
{ 
INT8U i;
if(n==FUN_OFF)      {for(i=0;i<LCD_BUFF_LEN;i++)  LCD_BUFF[i] =0x00; }
else if(n==FUN_ON)  {for(i=0;i<LCD_BUFF_LEN;i++)  LCD_BUFF[i] =0xFF; } 
}

void   Set_Hardware_Lcd_Blower(INT8U n)
{ 
if(n>=0)       { Lcdbitset(LCD_ID_BLW_LABLE);}
if(n>=1)       { Lcdbitset(LCD_ID_BLW1);}
if(n>=2)       { Lcdbitset(LCD_ID_BLW2);}
if(n>=3)       { Lcdbitset(LCD_ID_BLW3);}
if(n>=4)       { Lcdbitset(LCD_ID_BLW4);}
if(n>=5)       { Lcdbitset(LCD_ID_BLW5);}
if(n>=6)       { Lcdbitset(LCD_ID_BLW6);}
if(n>=7)       { Lcdbitset(LCD_ID_BLW7);}
}
void   Set_Hardware_Lcd_Mode(INT8U n)
{ 
 Lcdbitset(LCD_ID_BODY);
 if(n==FACE||n==FACE_FOOT) {Lcdbitset(LCD_ID_FACE);}
 if(n==FOOT||n==FOOT_DEF||n==FACE_FOOT)  {Lcdbitset(LCD_ID_FOOT);}
 if(n==DEF||n==FOOT_DEF)   {Lcdbitset(LCD_ID_DEF);}
}

void  Data1_Display( INT8U Data)
{
if     (Data==0)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);   Lcdbitset(LCD_ID_1C );  Lcdbitset(LCD_ID_1D ); Lcdbitset(LCD_ID_1E ); Lcdbitset(LCD_ID_1F);}
else if(Data==1)  {Lcdbitset(LCD_ID_1B);   Lcdbitset(LCD_ID_1C ); }
else if(Data==2)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);  Lcdbitset(LCD_ID_1G);  Lcdbitset(LCD_ID_1E);    Lcdbitset(LCD_ID_1D);}
else if(Data==3)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);  Lcdbitset(LCD_ID_1G);  Lcdbitset(LCD_ID_1C );   Lcdbitset(LCD_ID_1D);}
else if(Data==4)  {Lcdbitset(LCD_ID_1B);   Lcdbitset(LCD_ID_1G);  Lcdbitset(LCD_ID_1C ); Lcdbitset(LCD_ID_1F);}
else if(Data==5)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1F);  Lcdbitset(LCD_ID_1G);  Lcdbitset(LCD_ID_1C );   Lcdbitset(LCD_ID_1D);}
else if(Data==6)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1F);  Lcdbitset(LCD_ID_1G);  Lcdbitset(LCD_ID_1C );   Lcdbitset(LCD_ID_1D); Lcdbitset(LCD_ID_1E); }
else if(Data==7)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);  Lcdbitset(LCD_ID_1C ); }
else if(Data==8)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);  Lcdbitset(LCD_ID_1C ); Lcdbitset(LCD_ID_1D );   Lcdbitset(LCD_ID_1E ); Lcdbitset(LCD_ID_1F); Lcdbitset(LCD_ID_1G);}
else if(Data==9)  {Lcdbitset(LCD_ID_1A);   Lcdbitset(LCD_ID_1B);  Lcdbitset(LCD_ID_1C ); Lcdbitset(LCD_ID_1D );   Lcdbitset(LCD_ID_1F);  Lcdbitset(LCD_ID_1G);}
else {;}
}

void  Data2_Display( INT8U Data)
{
if(Data==0)       {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);   Lcdbitset(LCD_ID_2C );  Lcdbitset(LCD_ID_2D ); Lcdbitset(LCD_ID_2E ); Lcdbitset(LCD_ID_2F);}
else if(Data==1)  {Lcdbitset(LCD_ID_2B);   Lcdbitset(LCD_ID_2C ); }
else if(Data==2)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);  Lcdbitset(LCD_ID_2G);  Lcdbitset(LCD_ID_2E);    Lcdbitset(LCD_ID_2D);}
else if(Data==3)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);  Lcdbitset(LCD_ID_2G);  Lcdbitset(LCD_ID_2C );   Lcdbitset(LCD_ID_2D);}
else if(Data==4)  {Lcdbitset(LCD_ID_2B);   Lcdbitset(LCD_ID_2G);  Lcdbitset(LCD_ID_2C );   Lcdbitset(LCD_ID_2F);}
else if(Data==5)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2F);  Lcdbitset(LCD_ID_2G);  Lcdbitset(LCD_ID_2C );   Lcdbitset(LCD_ID_2D);}
else if(Data==6)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2F);  Lcdbitset(LCD_ID_2G);  Lcdbitset(LCD_ID_2C );   Lcdbitset(LCD_ID_2D); Lcdbitset(LCD_ID_2E); }
else if(Data==7)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);  Lcdbitset(LCD_ID_2C ); }
else if(Data==8)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);  Lcdbitset(LCD_ID_2C ); Lcdbitset(LCD_ID_2D );   Lcdbitset(LCD_ID_2E ); Lcdbitset(LCD_ID_2F); Lcdbitset(LCD_ID_2G);}
else if(Data==9)  {Lcdbitset(LCD_ID_2A);   Lcdbitset(LCD_ID_2B);  Lcdbitset(LCD_ID_2C ); Lcdbitset(LCD_ID_2D );   Lcdbitset(LCD_ID_2F);  Lcdbitset(LCD_ID_2G);}
else {;}
}


void  Data3_Display( INT8U Data)
{
if     (Data==0)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);   Lcdbitset(LCD_ID_3C );  Lcdbitset(LCD_ID_3D ); Lcdbitset(LCD_ID_3E ); Lcdbitset(LCD_ID_3F);}
else if(Data==1)  {Lcdbitset(LCD_ID_3B);   Lcdbitset(LCD_ID_3C ); }
else if(Data==2)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);  Lcdbitset(LCD_ID_3G);  Lcdbitset(LCD_ID_3E);    Lcdbitset(LCD_ID_3D);}
else if(Data==3)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);  Lcdbitset(LCD_ID_3G);  Lcdbitset(LCD_ID_3C );   Lcdbitset(LCD_ID_3D);}
else if(Data==4)  {Lcdbitset(LCD_ID_3B);   Lcdbitset(LCD_ID_3G);   Lcdbitset(LCD_ID_3C );   Lcdbitset(LCD_ID_3F);}
else if(Data==5)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3F);  Lcdbitset(LCD_ID_3G);  Lcdbitset(LCD_ID_3C );   Lcdbitset(LCD_ID_3D);}
else if(Data==6)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3F);  Lcdbitset(LCD_ID_3G);  Lcdbitset(LCD_ID_3C );   Lcdbitset(LCD_ID_3D); Lcdbitset(LCD_ID_3E); }
else if(Data==7)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);  Lcdbitset(LCD_ID_3C ); }
else if(Data==8)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);  Lcdbitset(LCD_ID_3C ); Lcdbitset(LCD_ID_3D );   Lcdbitset(LCD_ID_3E ); Lcdbitset(LCD_ID_3F); Lcdbitset(LCD_ID_3G);}
else if(Data==9)  {Lcdbitset(LCD_ID_3A);   Lcdbitset(LCD_ID_3B);  Lcdbitset(LCD_ID_3C ); Lcdbitset(LCD_ID_3D );   Lcdbitset(LCD_ID_3F);  Lcdbitset(LCD_ID_3G);}
else {;}
}


void   Set_Hardware_Lcd_Tem(INT16U n)
{
INT8U  i;
INT16U Data_First;
INT16U Data_Second;
INT16U Data_Third;

if(n>LCD_LO&&n<LCD_HI)
{
Data_Third= n%10;
Data_Second=(n/10)%10;
Data_First=n/100;

Lcdbitset(LCD_ID_TEM_UNIT);
Data1_Display( Data_First);
Data2_Display( Data_Second);
Data3_Display( Data_Third);
}
else if( n==LCD_HI )
{
Lcdbitset(LCD_ID_1F);Lcdbitset(LCD_ID_1E);Lcdbitset(LCD_ID_1G);Lcdbitset(LCD_ID_1B);Lcdbitset(LCD_ID_1C);
Lcdbitset(LCD_ID_2B);Lcdbitset(LCD_ID_2C);

}
else if(n==LCD_LO)
{
Lcdbitset(LCD_ID_1F);Lcdbitset(LCD_ID_1E);Lcdbitset(LCD_ID_1D);
Lcdbitset(LCD_ID_2A);Lcdbitset(LCD_ID_2F);Lcdbitset(LCD_ID_2E);Lcdbitset(LCD_ID_2D);Lcdbitset(LCD_ID_2B);Lcdbitset(LCD_ID_2C);

}
}

void   Set_Hardware_Lcd_Ac(INT8U n)
{ 
if(n==FUN_ON)
{Lcdbitset(LCD_ID_AC); }
}
void   Set_Hardware_Lcd_Rec(INT8U n)
{ 
if(n==INSIDE)       {Lcdbitset(LCD_ID_INSIDE ); }
else if(n==OUTSIDE) {Lcdbitset(LCD_ID_OUTSIDE);}
}

void   Set_Hardware_Lcd_Econ(INT8U n)
{  
if(n==FUN_ON) {Lcdbitset(LCD_ID_ECON);}           
}

void   Set_Hardware_Lcd_Auto(INT8U n)
{
 if(n==FUN_ON) {Lcdbitset(LCD_ID_AUTO);} 
}

void Out_Lcd_HT16C21(void)
{
Write_Page_Data(C_HT16C21_SET_ADD1,C_HT16C21_SET_ADD2, C_HT16C21_SET_ADD3);
}

void   Lcd_Proc(void)
{ 
  Set_Hardware_Lcd_Back(Set_Lcd_Backlight);

  Set_Hardware_Lcd_Fullon(FUN_OFF);    
         
  Set_Hardware_Lcd_Blower(Set_Lcd_Blw);
  Set_Hardware_Lcd_Mode(Set_Lcd_Modedrive);
  Set_Hardware_Lcd_Tem(Set_Lcd_Temp);
  Set_Hardware_Lcd_Ac(Set_Lcd_Acdrive);
  Set_Hardware_Lcd_Rec(Set_Lcd_Recdrive);
  Set_Hardware_Lcd_Econ(Set_Lcd_Econdrive);
  Set_Hardware_Lcd_Auto(Set_Lcd_Autodrive);
  Out_Lcd_HT16C21();
 
}

void Write_Byte_Comand( INT8U Command,  INT8U  Data_Setting)
{
IIC_Start();
IIC_Send8bit(C_HT16C21_SLAVE_ADDR); 
IIC_Send8bit(Command); 
IIC_Send8bit(Data_Setting);
IIC_Stop();
}

void  Write_Signal_Data(INT8U Add,INT8U Data)
{

IIC_Start();
IIC_Send8bit(C_HT16C21_SLAVE_ADDR); 
IIC_Send8bit(C_HT16C21_DATA_DSP); 
IIC_Send8bit(Add); 
IIC_Send8bit(Data); 
IIC_Stop();
}

void  Write_Page_Data(INT8U Add1, INT8U Add2, INT8U Add3)
{
INT8U i;
IIC_Start();
IIC_Send8bit(C_HT16C21_SLAVE_ADDR);
IIC_Send8bit(C_HT16C21_DATA_DSP);
IIC_Send8bit(Add1); 
for(i=0;i<2;i++)
{
IIC_Send8bit(LCD_BUFF[i]);
}
//IIC_Stop();
//IIC_Start();
//IIC_Send8bit(C_HT16C21_SLAVE_ADDR); 
//IIC_Send8bit(C_HT16C21_DATA_DSP); 

IIC_Send8bit(Add2); 
for(i=0;i<3;i++)
{
IIC_Send8bit(LCD_BUFF[i+2]);
} 
//IIC_Stop();

//IIC_Start();
//IIC_Send8bit(C_HT16C21_SLAVE_ADDR); 
//IIC_Send8bit(C_HT16C21_DATA_DSP); 
IIC_Send8bit(Add3); 
IIC_Send8bit(LCD_BUFF[5]);
IIC_Stop();
}


void Init_LCD(void)
{
Set_Hardware_Lcd_Fullon(FUN_OFF); 
Write_Byte_Comand(C_HT16C21_DRIVE_MODE,  C_HT16C21_DUTY_BIAS);
Write_Byte_Comand(C_HT16C21_FRAME_FREQ,  C_HT16C21_SET_FRAMEFRE);
Write_Byte_Comand(C_HT16C21_VOL_ADJ,     C_HT16C21_SET_VCLD);
Write_Byte_Comand(C_HT16C21_BLINK_FREQ,  C_HT16C21_SET_BLINK);
Write_Byte_Comand(C_HT16C21_SYSTEM_MODE, C_HT16C21_SET_SYSTEM );  
Write_Page_Data(C_HT16C21_SET_ADD1,C_HT16C21_SET_ADD2, C_HT16C21_SET_ADD3);
}


void  IIC_Start(void)
{
  PIR1bits.SSPIF=0;
  SSPCON2bits.SEN=1;
  Timer_IIC_Waitting=TIMER_IIC_WAIT;
  while((!PIR1bits.SSPIF)&&Timer_IIC_Waitting);  
  PIR1bits.SSPIF=0;        
 }

void  IIC_Stop(void)
{
  PIR1bits.SSPIF=0;
  SSPCON2bits.PEN=1;
  Timer_IIC_Waitting=TIMER_IIC_WAIT;
  while((!PIR1bits.SSPIF)&&Timer_IIC_Waitting);
  PIR1bits.SSPIF=0;
}

void  IIC_Send8bit(INT8U Data)
{    
  PIR1bits.SSPIF=0;
  SSPBUF=Data;
  Timer_IIC_Waitting=TIMER_IIC_WAIT;
  while((!PIR1bits.SSPIF)&&Timer_IIC_Waitting);
  PIR1bits.SSPIF=0;
}

/*initial function for MCU core
*********************************************************************/
void Init_MCU_IO(void)
{
//When the io is applied AD input ,the TRISA must be set 1.
//when RA6,RA7 are connnect to external cystal ,the TRISA ,LAT must be set 0.
TRISA=MARK_PORTA;
TRISB=MARK_PORTB;
TRISC=MARK_PORTC;
TRISD=MARK_PORTD;
TRISE=MARK_PORTE;
TRISF=MARK_PORTF;
TRISG=MARK_PORTG;

PORTA=0X00;
PORTB=0X00;
PORTC=0X00;
PORTD=0X00;
PORTE=0X00;
PORTF=0X00;
PORTG=0X00;

LATA=0X00;
LATB=0X00;
LATC=0X00;
LATD=0X00;
LATE=0X00;
LATF=0X00;
LATG=0X00;

Inside_Tem_Init();
Outside_Tem_Init();
Sun_Sensor_Init();
Evap_Sensor_Init();

Module_Feedback_Init();
Power_Feedback_Init();
Motor_Tem_Init();
Motor_Mode_Init();

}

void Init_MCU_Clock(void)
{

OSCTUNEbits.PLLEN=0;

}

void Init_MCU_Timer(void)
{

PMD1bits.TMR1MD=0;       //enable the timer1
T1GCON    = 0X00;  
T1CON     = 0X06;     
   
RCONbits.IPEN   = 1;  
PIE1bits.TMR1IE = 1;   
IPR1bits.TMR1IP = 1;  
INTCONbits.GIEH =1;  
T1CONbits.TMR1ON =1;

}

void Init_MCU_AD(void)
{

PMD1bits.ADCMD  = 0;       
ADCON0          = 0x00;
ADCON1 =0X00;              
ADCON2 =0XA0;             
PIR1bits.ADIF=0;        
PIE1bits.ADIE=0;        
ANCON0 =0X00;        
ANCON1= 0X00; 
ANCON0=MARK_ADL;
ANCON1=MARK_ADH;        

}

void Init_MCU_PWM(void)
{

 PR4 = C_PWM_MAX;
 T4CON =   0X04;   
 CCP3CON = 0x0c;  
 CCPR3L =  0x00;   
 CCPTMRS = 0x04;
 PIE4bits.CCP3IE =0;
 //OUT_MODULE_PROTECT=1;
 CM1CON = 0X00;    //Compare funtion is disable
 CM2CON = 0X00;    //Compare funtion is disable

}

void Init_WatchDog(void)
{
 WDTCON = 0x01;   //The watchdog is on
}


void  Init_I2c(void)
{
TRISCbits.TRISC3=1;
TRISCbits.TRISC4=1;
//SSPSTAT=0X80;
SSPCON1=0X28;
SSPCON2=0X00;
SSPADD=9;
PIR1bits.SSPIF=0;
}
/*********************************************************************/