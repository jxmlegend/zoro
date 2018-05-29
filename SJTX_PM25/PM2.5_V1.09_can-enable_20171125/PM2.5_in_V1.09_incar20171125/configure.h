#ifndef   _configure_h
#define   _configure_h

//#include  <p18f66K80.h>
#include  "p18f25k80.h"


#include "const.h"


#define    C_AD_CH0       0X00
#define    C_AD_CH1       0X04
#define    C_AD_CH2       0X08
#define    C_AD_CH3       0X0C
#define    C_AD_CH4       0X10
#define    C_AD_CH5       0X14
#define    C_AD_CH6       0X18
#define    C_AD_CH7       0X1C
#define    C_AD_CH8       0X20
#define    C_AD_CH9       0X24
#define    C_AD_CH10      0X28

#define    MARK_ADL       0XFF
#define    MARK_ADH       0X00

/*
***************************************************************************************************************************/
/*configure system
***************************************************************************************************************************/








#define    CH_OUTSIDE_TEM          C_AD_CH3
#define    CH_INSIDE_TEM           C_AD_CH4
#define    CH_EVAP_SENSOR          C_AD_CH1
#define    CH_SUN_SENSOR           C_AD_CH2
#define    CH_MODE_FB              C_AD_CH6
#define    CH_MIX_FB               C_AD_CH5
#define    CH_MODULE_FB            C_AD_CH7
#define    CH_IGN_FB               C_AD_CH0
#define    CH_BAT_FB               C_AD_CH1
//#define  CH_ILLUMI_FB            C_AD_CH0





#endif