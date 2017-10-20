#include "PID.h"
#include "lcd.h"

pid_InitTypeDef pid;

void PID_init()
{						
    pid.SetSpeed=0.0;//定义设定值
    pid.ActualSpeed=0.0;//定义实际值
    pid.err=0.0;//定义偏差值
    pid.err_next=0.0;//定义上一个偏差值
		pid.err_last=0.0;//最上前的偏差值
    pid.voltage=0.0;//定义电压值（控制执行器的变量）
		pid.integral=0;
	//定义比例、积分、微分系数
    pid.Kp=1.1;//2.28 1.42 1.22 1.1
    pid.Ki=0.1;//0.03 0.1
    pid.Kd=0.02;//0.01 0.02
}

//float PID_realize(float speed)
//{
//    pid.SetSpeed=speed;
//    pid.err=pid.SetSpeed-pid.ActualSpeed;//偏差值
//    pid.integral+=pid.err;//积分值
//    pid.voltage=pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err-pid.err_last);
//    pid.err_last=pid.err;
//    pid.ActualSpeed=pid.voltage*1.0;
//    return pid.ActualSpeed;//返回实际值
//}


//float PID_realize(float speed,float Actual)
//{
//    pid.SetSpeed=speed;//用户设置的值
//		pid.ActualSpeed=Actual;
//		LCD_ShowxNum(30,250,pid.SetSpeed,4,16,0x80);
//    pid.err=pid.SetSpeed-pid.ActualSpeed;//偏差值
//		LCD_ShowxNum(80,250,pid.ActualSpeed,4,16,0x80);
//		LCD_ShowxNum(118,250,pid.err,4,16,0x80);

////    if(abs(pid.err)>200)           //变积分过程
////    {
////			index=0.0;
////    }
////		else if(abs(pid.err)<180)
////		{
////			index=1.0;
////			pid.integral+=pid.err;
////    }
////		else
////		{
////			index=(200-abs(pid.err))/20;
////			pid.integral+=pid.err;
////    }
//    pid.voltage=pid.Kp*(pid.err-pid.err_next)+pid.Ki*pid.err+pid.Kd*(pid.err-2*pid.err_next+pid.err_last);//pid.Kp*pid.err+ pid.Kd*(pid.err-pid.err_last);
//		pid.voltage+=pid.ActualSpeed;
//    pid.err_last=pid.err_next;
//    pid.err_next=pid.err;
//	
//    //pid.ActualSpeed=pid.voltage*1.0;
//    return pid.voltage;//返回实际调整值
//}

float PID_realize(float speed,float ActualSpeed)
{
	float index;
	pid.SetSpeed=speed;
	LCD_ShowxNum(30,250,pid.SetSpeed,4,16,0x80);
	pid.ActualSpeed=ActualSpeed;
	pid.err=pid.SetSpeed-pid.ActualSpeed;
	LCD_ShowxNum(80,250,pid.ActualSpeed,4,16,0x80);
	LCD_ShowxNum(118,250,pid.err,4,16,0x80);
	if(abs(pid.err)>90)
	{
		index=0.0;
	}
	else if(abs(pid.err)<81)
	{
		index=1.0;
		if(pid.err>0)
		pid.integral+=pid.err;
		else
		pid.integral-=abs(pid.err);
	}
	else
	{
		index=(90-abs(pid.err))/9;
		if(pid.err>0)
		pid.integral+=pid.err;
		else
		pid.integral-=abs(pid.err);
	}
	pid.voltage=pid.Kp*pid.err+index*pid.Ki*pid.integral/2+pid.Kd*(pid.err-pid.err_last);
	pid.voltage=pid.voltage+pid.ActualSpeed;
	pid.err_last=pid.err_next;
	pid.err_next=pid.err;
	return pid.voltage;
}
