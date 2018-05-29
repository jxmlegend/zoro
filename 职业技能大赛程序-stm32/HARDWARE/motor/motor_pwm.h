#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define  MOTOR_A_CON1   PCout(0)
#define  MOTOR_A_CON2   PCout(1)
#define  MOTOR_B_CON1   PCout(2)
#define  MOTOR_B_CON2   PCout(3)

#define MOTOR_GO_FORWARD  {MOTOR_A_CON1=0;MOTOR_A_CON2=1;MOTOR_B_CON1=1;MOTOR_B_CON2=0;} //车体前进	                            
#define MOTOR_GO_BACK	    {MOTOR_A_CON1=1;MOTOR_A_CON2=0;MOTOR_B_CON1=0;MOTOR_B_CON2=1;}//车体后退
#define MOTOR_GO_RIGHT	  {MOTOR_A_CON1=0;MOTOR_A_CON2=1;MOTOR_B_CON1=0;MOTOR_B_CON2=1;}//车体左转
#define MOTOR_GO_LEFT	    {MOTOR_A_CON1=1;MOTOR_A_CON2=0;MOTOR_B_CON1=1;MOTOR_B_CON2=0;}//车体右转
#define MOTOR_GO_STOP	    {MOTOR_A_CON1=0;MOTOR_A_CON2=0;MOTOR_B_CON1=0;MOTOR_B_CON2=0;} //车体停止
void MotorInit(u16 arr,u16 psc);
void SetMotorPWM(u16 PWM1,u16 PWM2);

#endif
