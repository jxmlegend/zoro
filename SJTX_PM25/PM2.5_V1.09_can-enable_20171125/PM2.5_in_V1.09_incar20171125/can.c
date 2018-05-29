#include   "can.h"
#include   "input.h"
#include   "const.h"

INT16U        CAN_RXMSG_POS;
INT16U        CAN_RXMSG_LEN;

CAN_DRIVER    CANnode;
CAN_RXMSG     RxMsg_AC;
static        CAN_RXMSG     CAN_RXMSG_BUFF[C_CAN_RXMSG_MAXLEN];

INT16U       Check_BUG;




                                             


INT8U      CANT_Pm25OutComStatus;

 
void       PutMsg_Diag_Pm25OutComStatus(INT8U Sts)      {    CANT_Pm25OutComStatus             =Sts;  }





INT8U      CAN_GetSatus(INT8U  Channel)
{
  INT8U n;
  if(Channel== Pm25_Out_Sensor)
  { 
     n = CANR_Pm25OutError;
     return n;
  }
  else
  {
    n=0;        // "0" need to be defined
    return n;
  }
} 


void CAN_Put_RxMsg(void)
{
   INT16U   Id;
   Id=   RXB0SIDH;
   Id=   (Id<< 8) + RXB0SIDL; 
   

   if((RXB0SIDL&0B00001000)==0B00000000)
   {
      
      CAN_RXMSG_BUFF[CAN_RXMSG_LEN].ID       =  Id>>5;
      CAN_RXMSG_BUFF[CAN_RXMSG_LEN].Extend    =  0; 
   }
   else
   {
      CAN_RXMSG_BUFF[CAN_RXMSG_LEN].Extend    =  1; 
   }

     CAN_RXMSG_BUFF[CAN_RXMSG_LEN].Length       =  RXB0DLC & 0x0F;
   
  

   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[0]    =  RXB0D0;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[1]    =  RXB0D1;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[2]    =  RXB0D2;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[3]    =  RXB0D3;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[4]    =  RXB0D4;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[5]    =  RXB0D5;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[6]    =  RXB0D6;
   CAN_RXMSG_BUFF[CAN_RXMSG_LEN].RxData[7]    =  RXB0D7;

   CAN_RXMSG_LEN   =  (CAN_RXMSG_LEN+1)%C_CAN_RXMSG_MAXLEN;

}

INT16U CAN_Get_RxMsg(void)
{

if(CAN_RXMSG_POS != CAN_RXMSG_LEN)            /*for this stentance need to be think deeply*/
{  
   RxMsg_AC.ID        =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].ID;
   RxMsg_AC.Extend    =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].Extend;
   RxMsg_AC.Length    =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].Length;

   RxMsg_AC.RxData[0] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[0];
   RxMsg_AC.RxData[1] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[1];
   RxMsg_AC.RxData[2] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[2];
   RxMsg_AC.RxData[3] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[3];
   RxMsg_AC.RxData[4] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[4];
   RxMsg_AC.RxData[5] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[5];
   RxMsg_AC.RxData[6] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[6];
   RxMsg_AC.RxData[7] =  CAN_RXMSG_BUFF[CAN_RXMSG_POS].RxData[7];
	   	   	   
   CAN_RXMSG_POS      =  (CAN_RXMSG_POS+1)%C_CAN_RXMSG_MAXLEN;   
   return 1;
}
else
{
   return 0;
}

}




void CAN_Init_RxMsg(void)
{
   CAN_RXMSG_POS  =  0;
   CAN_RXMSG_LEN  =  0;   
}

void CAN_Init(void)
{
   CANnode.Status         =  C_CAN_STA_INIT0;   
}


void CAN_pro( )
{

INT16U  n;


/*
      CANnode.Pm25OutStatus =  C_ECU_OK;
      
      CANnode.InitCount     =C_CAN_INITCOUNT;
      CANnode.InitTimeoutCnt=C_CAN_INITTIMEOUT;
      CANnode.Status        =C_CAN_STA_INIT1;  
       
      CANnode.BusOff        =0;      
      CANnode.BusOffCount   =0;
*/     
      
     if(CANR_FLAG == 1)			       	 
     {          
        CAN_Put_RxMsg();      
       // CANR_FLAG   =0;		            			    		
       

       for(n=0;n<20;n++)
       { 
         if(CAN_Get_RxMsg())
         {
           if(RxMsg_AC.Extend==0)
           {     
             switch (RxMsg_AC.ID)
             {    
               case    ID_CANRX_MSG_Pm25Out: 
                {

                CANnode.Pm25OutTimeoutCnt=C_TIMER_Pm25OutTIMEROUT; 
                CANnode.Pm25OutStatus=C_Pm25Out_OK;
                CANR_OUT_OK = CANnode.Pm25OutStatus;
                
                CANR_Pm25OutSn = (RxMsg_AC.RxData[1]& 0x3E)>>1;
                
                CANR_Pm25OutValue =RxMsg_AC.RxData[0]*4+((RxMsg_AC.RxData[1]& 0xE0 )>>6);
                
                PutMsg_Diag_Pm25OutComStatus(0);  
               
                }
              break;       
       

               default :
               break;
             }
            }
            else   { ; }
       
          }
          else   { ; }  
        }
        CANR_FLAG   =0;	 
      }
     
       if(1)                                   //*!!!!!!!!!!!!!!!! system on status ********  /                 
       { 
         if(CANnode.Pm25OutTimeoutCnt)       CANnode.Pm25OutTimeoutCnt -=  C_SYSTEM_TIMER_INTERVAL;

         if(CANnode.Pm25OutTimeoutCnt==0)  
         {
           if(CANnode.Pm25OutStatus==C_ECU_OK)
           { 
             CANnode.Pm25OutStatus  = C_ECU_NG ;
             PutMsg_Diag_Pm25OutComStatus(1); 
           }
           CANnode.Pm25OutTimeoutCnt=C_TIMER_Pm25OutTIMEROUT;
         }
       }
      
       else 
       {
        CANnode.Pm25OutTimeoutCnt=C_TIMER_Pm25OutTIMEROUT;
       }
   CANR_Pm25OutError=CANnode.Pm25OutStatus;
   CANR_OUT_OK ==CANnode.Pm25OutStatus;

}

