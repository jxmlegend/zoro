/****************************************
文件名称： 四驱智能小车V4.0程序
时间：2015年8月11日
版本：V4.0
说明：
修改记录：
****************************************/  
#ifndef _MOTOR_H_
#define _MOTOR_H_

 
 
#include "commom.h"

void Motor_Init(void);//声明电机驱动函数

/*小车电机接法：
电机 “+”端接红色线，“-”端接黑色线
小车左边两个电机：红色线接1Y1，黑色线接1Y2
小车右边两个电机：红色线接2Y1，黑色线接2Y2
																			(从轮胎一侧看)
MOTOR_A_CON1=1;MOTOR_A_CON2=0; 左电机前进 	逆时针 正转
MOTOR_B_CON1=0;MOTOR_B_CON2=0; 右电机前进 	顺时针 反转
MOTOR_A_CON1=0;MOTOR_A_CON2=1; 左电机后退  顺时针 反转
MOTOR_B_CON1=1;MOTOR_B_CON2=0; 右电机后退  逆时针 正转
*/

#define MOTOR_GO_FORWARD {MOTOR_A_CON1=1;MOTOR_A_CON2=0;MOTOR_B_CON1=0;MOTOR_B_CON2=1;} //车体前进   
#define MOTOR_GO_BACK	   {MOTOR_A_CON1=0;MOTOR_A_CON2=1;MOTOR_B_CON1=1;MOTOR_B_CON2=0;}//车体后退
#define MOTOR_GO_LEFT	   {MOTOR_A_CON1=0;MOTOR_A_CON2=1;MOTOR_B_CON1=0;MOTOR_B_CON2=1;}//车体左转 
#define MOTOR_GO_RIGHT	 {MOTOR_A_CON1=1;MOTOR_A_CON2=0;MOTOR_B_CON1=1;MOTOR_B_CON2=0;}//车体右转
#define MOTOR_GO_STOP	   {MOTOR_A_CON1=0;MOTOR_A_CON2=0;MOTOR_B_CON1=0;MOTOR_B_CON2=0;} //车体停止


#endif

