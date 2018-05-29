#include  "input.h"
#include  "const.h"
#include  "configure.h"
#include  "p18f25k80.h"

INT8U Test_Point01;                       // for code test

SUPPLY_VOLTAGE_CONTROL   IGN_Feedback;

SUPPLY_VOLTAGE_CONTROL   BATT_Feedback;

void Init_MCU_AD(void)
{

  PMD1bits.ADCMD  = 0;       
  ADCON0          = 0x00;
  ADCON1 =0X30;               //ORG:00               
  ADCON2 =0XA0;              //”“∂‘∆Î       
  PIR1bits.ADIF=0;        
  PIE1bits.ADIE=0;        
  ANCON0 =0X00;        
  ANCON1= 0X00; 
  ANCON0=MARK_ADL;
  ANCON1=MARK_ADH;        
}
        
INT16U Get_AD_Word(INT8U CH)
{
	INT16U n;
	ADCON0 = CH;
	ADCON0bits.ADON = 1;
	for(n=0;n<50;n++)
	{
        Nop();	
	}
	ADCON0bits.GO = 1;
	while(ADCON0bits.GO);
	n = ADRESH*256 + ADRESL; 
	ADCON0bits.ADON = 0;
    n >>=2;
	return n; 
}

//**********************************************************
INT16U   Feedback_GetValue(INT8U Channel)
{

if (Channel==BATT_FEEDBACK)         return   BATT_Feedback.Value_ECU;
else  if (Channel==IGN_FEEDBACK)          return   IGN_Feedback.Value_ECU;

else return 0 ;
}


INT8U      Feedback_GetSatus(INT8U Channel)
{
  if (Channel==BATT_FEEDBACK)
    return   BATT_Feedback.Status;
  else  if (Channel==IGN_FEEDBACK)
     return   IGN_Feedback.Status;
  else     
      return 0;
}

//********************************************/

void     Input_Sensor_Proc(void)
{

   IGN_Feedback_Detect();
   
//   Test_Point01=IGN_Feedback.Status;
   
   BATT_Feedback_Detect();
   
}


INT16U  IGN_Feedback_AdtoVoltage(INT16U  AdValue)
{  return  AdValue;    }

INT16U  BATT_Feedback_AdtoVoltage(INT16U AdValue)
{  return  AdValue;    }


/**********************************************************************************************************/

/* IGN input and operating 
*********************************************************************************************************/
void IGN_Feedback_Init( )
{

 IGN_Feedback.Init=1;
 IGN_Feedback.Status=C_IGN_NORMAL ;
 IGN_Feedback.Interval=C_AD_MAX_NUM *C_SYSTEM_TIMER_INTERVAL;
 //IGN_Feedback.Value_ECU=0XFF;           /**TO think deeply*****/

}


void IGN_Feedback_Detect()
{

  INT16U  AD, DAT;
  INT8U i;
  AD=Get_AD_Word(CH_IGN_FB);
  DAT=  IGN_Feedback_AdtoVoltage(AD);
  
  if(IGN_Feedback.Init)
  {
    IGN_Feedback.Init=0;
    IGN_Feedback.Status=C_IGN_NORMAL;
    IGN_Feedback.Interval=C_AD_MAX_NUM*C_SYSTEM_TIMER_INTERVAL;
    IGN_Feedback.TimerOutCountOverVoltage =C_TI_IGN_O_V;
    IGN_Feedback.TimerOutCountUnderVoltage=C_TI_IGN_U_V ;
    IGN_Feedback.Value_SUM=0;
    IGN_Feedback.Count=0;

    for (i=0;i<C_SMOOTHING_COUNT;i++)
    {
      IGN_Feedback.Buffer[i]=DAT;
      IGN_Feedback.Value_SUM +=DAT;
    }
   }
   
//**************************************************************************************
  IGN_Feedback.Value_SUM  -= IGN_Feedback.Buffer[IGN_Feedback.Count];
  IGN_Feedback.Value_SUM  += DAT;
  IGN_Feedback.Buffer[IGN_Feedback.Count]= DAT;
  IGN_Feedback.Count =(IGN_Feedback.Count+1)%C_SMOOTHING_COUNT;

   DAT=(INT16U)IGN_Feedback.Value_SUM/C_SMOOTHING_COUNT;

  IGN_Feedback.Value_ECU=DAT;

  i=0;
  if(i)       //when the head if off ,the voltage diagnostic is not applied
   {
    IGN_Feedback.Status=C_IGN_NORMAL;
    IGN_Feedback.TimerOutCountOverVoltage =C_TI_IGN_O_V;
    IGN_Feedback.TimerOutCountUnderVoltage=C_TI_IGN_U_V ;
    return ;
   }

 switch   (IGN_Feedback.Status)
 {
  case   C_IGN_NORMAL:

       if(IGN_Feedback.Value_ECU>C_V_TH_IGN_O_V)
       {
         if( IGN_Feedback.TimerOutCountOverVoltage> IGN_Feedback.Interval )
           { IGN_Feedback.TimerOutCountOverVoltage -=IGN_Feedback.Interval; }
         else 
           { IGN_Feedback.TimerOutCountOverVoltage =0; }

         if(IGN_Feedback.TimerOutCountOverVoltage ==0)
         {
                                              //!!!!!!speed  is over a value the overtage is effect
          IGN_Feedback.Status=C_IGN_0VERVOLTAGE;
          IGN_Feedback.TimerOutCountReturnVoltage=C_TI_IGN_O_V_R ;
                                              //system off
         }
       }
       else 
        {
         IGN_Feedback.TimerOutCountOverVoltage=C_TI_IGN_O_V;
        }

       if(IGN_Feedback.Value_ECU < C_V_TH_IGN_U_V)
       {
         if( IGN_Feedback.TimerOutCountUnderVoltage> IGN_Feedback.Interval )
           { IGN_Feedback.TimerOutCountUnderVoltage -=IGN_Feedback.Interval; }
         else 
          { IGN_Feedback.TimerOutCountUnderVoltage =0; }

         if(IGN_Feedback.TimerOutCountUnderVoltage ==0)
         {
 
                      //!!!!!!!!!!!!!!enginee is on, a value the overtage is effect
          IGN_Feedback.Status=C_IGN_UNDERVOLTAGE;
          IGN_Feedback.TimerOutCountReturnVoltage=C_TI_IGN_U_V_R;
                       //system off
         }
       }
       else 
        {
         IGN_Feedback.TimerOutCountUnderVoltage=C_TI_IGN_U_V;
        }
       break;


 case     C_IGN_UNDERVOLTAGE:

       if(IGN_Feedback.Value_ECU>=C_V_TH_IGN_U_V_R)
       {
        if(IGN_Feedback.TimerOutCountReturnVoltage>IGN_Feedback.Interval)
         {IGN_Feedback.TimerOutCountReturnVoltage -= IGN_Feedback.Interval;}
        else {IGN_Feedback.TimerOutCountReturnVoltage=0;}

        if(IGN_Feedback.TimerOutCountReturnVoltage==0)
         {
          IGN_Feedback.Status=C_IGN_NORMAL;
                                    //system on
          IGN_Feedback.TimerOutCountOverVoltage =C_TI_IGN_O_V;
          IGN_Feedback.TimerOutCountUnderVoltage=C_TI_IGN_U_V ;
          //PutMsg_Diag_IGNStatus(0);
         }
       }
       else 
       {

         IGN_Feedback.TimerOutCountReturnVoltage=C_TI_IGN_U_V_R;

       }

       break;

 case     C_IGN_0VERVOLTAGE: 

      if(IGN_Feedback.Value_ECU<=C_V_TH_IGN_O_V_R)
      {
        if(IGN_Feedback.TimerOutCountReturnVoltage>IGN_Feedback.Interval)
         {IGN_Feedback.TimerOutCountReturnVoltage -= IGN_Feedback.Interval;}
        else{IGN_Feedback.TimerOutCountReturnVoltage=0;}

        if(IGN_Feedback.TimerOutCountReturnVoltage==0)
        {

          IGN_Feedback.Status=C_IGN_NORMAL;
                             //system on
          IGN_Feedback.TimerOutCountOverVoltage  = C_TI_IGN_O_V;
          IGN_Feedback.TimerOutCountUnderVoltage = C_TI_IGN_U_V ;
           //PutMsg_Diag_IGNStatus(0);
        }

      }
      else 
      {

        IGN_Feedback.TimerOutCountReturnVoltage=C_TI_IGN_O_V_R;

      }
      break;     

 default:  

 IGN_Feedback.Status=C_IGN_NORMAL;

 break;

 }
// ***************************************************************************************/
}


/**********************************************************************************************************/

/* BATT input and operating 
*********************************************************************************************************/
void BATT_Feedback_Init( )
{	
  BATT_Feedback.Init=1;
  BATT_Feedback.Status=C_BATT_NORMAL ;
  BATT_Feedback.Interval=C_AD_MAX_NUM *C_SYSTEM_TIMER_INTERVAL;
  //BATT_Feedback.Value_ECU=0XFF;           /**TO think deeply*****/
}


void BATT_Feedback_Detect()
{
  INT16U  AD, DAT;
  INT8U i;

  AD=Get_AD_Word(CH_BAT_FB);

  DAT=  BATT_Feedback_AdtoVoltage(AD);

  if(BATT_Feedback.Init)
  {
   BATT_Feedback.Init=0;
   BATT_Feedback.Status=C_BATT_NORMAL;
   BATT_Feedback.Interval=C_AD_MAX_NUM*C_SYSTEM_TIMER_INTERVAL;
   BATT_Feedback.TimerOutCountOverVoltage =C_TI_BATT_O_V;
   BATT_Feedback.TimerOutCountUnderVoltage=C_TI_BATT_U_V ;
   BATT_Feedback.Value_SUM=0;
   BATT_Feedback.Count=0;

   for (i=0;i<C_SMOOTHING_COUNT;i++)
   {
    BATT_Feedback.Buffer[i]=DAT;
    BATT_Feedback.Value_SUM +=DAT;
   }
  }

  BATT_Feedback.Value_SUM  -= BATT_Feedback.Buffer[BATT_Feedback.Count];
  BATT_Feedback.Value_SUM  += DAT;
  BATT_Feedback.Buffer[BATT_Feedback.Count]= DAT;
  BATT_Feedback.Count =(BATT_Feedback.Count+1)%C_SMOOTHING_COUNT;

  DAT=(INT16U)BATT_Feedback.Value_SUM/C_SMOOTHING_COUNT;

  BATT_Feedback.Value_ECU=DAT;

  i=0;
  if(i)
              //when the head if off ,the voltage diagnostic is not applied
  {
   BATT_Feedback.Status=C_BATT_NORMAL;
   BATT_Feedback.TimerOutCountOverVoltage =C_TI_BATT_O_V;
   BATT_Feedback.TimerOutCountUnderVoltage=C_TI_BATT_U_V ;
   return ;
  }

 switch   (BATT_Feedback.Status)
 {
  case   C_BATT_NORMAL:

        if(BATT_Feedback.Value_ECU>C_V_TH_BATT_O_V)
        {
          if( BATT_Feedback.TimerOutCountOverVoltage> BATT_Feedback.Interval )
           { BATT_Feedback.TimerOutCountOverVoltage -=BATT_Feedback.Interval; }
          else 
           { BATT_Feedback.TimerOutCountOverVoltage =0; }

          if(BATT_Feedback.TimerOutCountOverVoltage ==0)
             {

                                               //!!!!!!speed  is over a value the overtage is effect
               BATT_Feedback.Status=C_BATT_0VERVOLTAGE;
               BATT_Feedback.TimerOutCountReturnVoltage=C_TI_BATT_O_V_R ;
                                               //system off
             }
        }
        else 
        {
         BATT_Feedback.TimerOutCountOverVoltage=C_TI_BATT_O_V;
        }


        if(BATT_Feedback.Value_ECU < C_V_TH_BATT_U_V)
        {
          if( BATT_Feedback.TimerOutCountUnderVoltage> BATT_Feedback.Interval )
           { BATT_Feedback.TimerOutCountUnderVoltage -=BATT_Feedback.Interval; }
          else 
           { BATT_Feedback.TimerOutCountUnderVoltage =0; }

          if(BATT_Feedback.TimerOutCountUnderVoltage ==0)
           {
                      //!!!!!!!!!!!!!!enginee is on, a value the overtage is effect
             BATT_Feedback.Status=C_BATT_UNDERVOLTAGE;
             BATT_Feedback.TimerOutCountReturnVoltage=C_TI_BATT_U_V_R;
                       //system off
           }
        }
        else 
        {
         BATT_Feedback.TimerOutCountUnderVoltage=C_TI_BATT_U_V;
        }
        break;


  case     C_BATT_UNDERVOLTAGE:

        if(BATT_Feedback.Value_ECU>=C_V_TH_BATT_U_V_R)
        {
          if(BATT_Feedback.TimerOutCountReturnVoltage>BATT_Feedback.Interval)
            {BATT_Feedback.TimerOutCountReturnVoltage -= BATT_Feedback.Interval;}
          else {BATT_Feedback.TimerOutCountReturnVoltage=0;}

          if(BATT_Feedback.TimerOutCountReturnVoltage==0)
           {
             BATT_Feedback.Status=C_BATT_NORMAL;
                                                      //system on
             BATT_Feedback.TimerOutCountOverVoltage =C_TI_BATT_O_V;
             BATT_Feedback.TimerOutCountUnderVoltage=C_TI_BATT_U_V ;
           }
        }
        else 
        {

         BATT_Feedback.TimerOutCountReturnVoltage=C_TI_BATT_U_V_R;

        }

       break;

 case     C_BATT_0VERVOLTAGE: 

     if(BATT_Feedback.Value_ECU<=C_V_TH_BATT_O_V_R)
     {

      if(BATT_Feedback.TimerOutCountReturnVoltage>BATT_Feedback.Interval)
       {BATT_Feedback.TimerOutCountReturnVoltage -= BATT_Feedback.Interval;}
      else{BATT_Feedback.TimerOutCountReturnVoltage=0;}

      if(BATT_Feedback.TimerOutCountReturnVoltage==0)
       {

        BATT_Feedback.Status=C_BATT_NORMAL;
                                                                 //system on
        BATT_Feedback.TimerOutCountOverVoltage  = C_TI_BATT_O_V;
        BATT_Feedback.TimerOutCountUnderVoltage = C_TI_BATT_U_V ;

       }

     }
     else 
     {

      BATT_Feedback.TimerOutCountReturnVoltage=C_TI_BATT_O_V_R;

     }
    break;     

 default:  

  BATT_Feedback.Status=C_BATT_NORMAL;

 break;

 }
}

/***************************************************************************************************/

