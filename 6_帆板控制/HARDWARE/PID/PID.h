#ifndef __PID_H
#define __PID_H	
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "stdlib.h"
typedef struct _pid
{
    float SetSpeed;            //定义设定值
    float ActualSpeed;        //定义实际值
    float err;                //定义偏差值
    float err_next;            //定义上一个偏差值
		float err_last;            //定义最上前的偏差值
    float Kp,Ki,Kd;            //定义比例、积分、微分系数
    float voltage; //定义电压值（控制执行器的变量）
		float integral;         
 
}pid_InitTypeDef;

void PID_init(void);
float PID_realize(float,float);


#endif
