/****************************************
文件名称： 四驱智能小车V4.0程序
时间：2015年8月11日
版本：V4.0
说明：
修改记录：
****************************************/  
#include "motor.h"
#include "delay.h"

// #include "timer.h"

/**电机初始化**/
void Motor_Init(void)
{
	MOTOR_A_EN=0;
	MOTOR_B_EN=0;
	MOTOR_GO_STOP;
}
