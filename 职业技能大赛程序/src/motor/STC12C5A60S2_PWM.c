#include "STC12C5A60S2_PWM.h"
#include <STC12C5A60S2.H>


void PWM_ini()
{
	CMOD=0x0C;//定时器0溢出率作为时钟输入，选择FOSC/6作为时钟源,11.0592MHZ晶振时，PWM频率=11059200/6/256=7.2Khz        
	CL=0x00;//清零自由递增计数的16位定时器的值 CL的值是随PCA的时钟输入递增的，当CL的值等于或大于｛EPCnL，CCAPnL｝时，输出为高。
	CH=0x00;//CH0为00  看结构图，CL前面是永远是0

    /***PWM0相关寄存器设置***/
	PCA_PWM0=0x00; //EPC0H=0,EPC0L=0 EPC0H和CCAP0H组合成捕获寄存器｛EPC0H，CCAP0H｝
	//                               EPC0L和CCAP0L组合成捕获寄存器｛EPC0L，CCAP0L｝
					//	PCA_PWMn: 7 6 5 4 3 2 1 0
					//; - - - - - - EPCnH EPCnL
	CCAPM0=0x42;//设置ECOM1=1,PWM1=1.
	            //PCA模块0的工作模式寄存器
                //---------------------------------------
                //7   6    5     4     3    2    1    0
                //- ECM0 CAPP0 CAPN0 MAT0 T0G0 PWM0 ECCF0
                //----1----0-----0----0-----0---1-----0---
                //ECOM0:使能比较器，1时使能比较器功能
                //CAPP0：正捕获，1时使能上升沿捕获
                //CAPN0：负捕获，1时使能下降沿捕获
                //MAT0:匹配:1时，PCA计数器的值与模块的比较/捕获寄存器的值匹配将置位CCON寄存器中断标志位CCFn
                //T0G0:翻转，1时，工作在PCA高速输出模式，PCA计数器的值与模块的比较/捕获寄存器的值匹配将是CEXn脚翻转
                //PWM0:脉宽调节输出模式，1时，使能CEXn脚用做PWM输出
                //EECF0:使能CCFn中断，使能寄存器CCON中的捕获/比较标志CCFn，用来产生中断
    /***PWM1相关寄存器设置***/
  PCA_PWM1=0x00;//EPC1H=0,EPC1L=0 EPC1H和CCAP0H组合成捕获寄存器｛EPC1H，CCAP1H｝
					//EPC1L和CCAP0L组合成捕获寄存器｛EPC1L，CCAP1L｝
	                //	PCA_PWMn: 7 6 5 4 3 2 1 0
					//; - - - - - - EPCnH EPCnL 
	CCAPM1=0x42;//设置ECOM1=1,PWM1=1.
	            //PCA模块0的工作模式寄存器
                //---------------------------------------
                //7   6    5     4     3    2    1    0
                //- ECM1 CAPP1 CAPN1 MAT1 T0G1 PWM1 ECCF1
                //----1----0-----0----0-----0---1-----0---
                //ECOM1:使能比较器，1时使能比较器功能
                //CAPP1：正捕获，1时使能上升沿捕获
                //CAPN1：负捕获，1时使能下降沿捕获
                //MAT1:匹配:1时，PCA计数器的值与模块的比较/捕获寄存器的值匹配将置位CCON寄存器中断标志位CCFn
                //T0G1:翻转，1时，工作在PCA高速输出模式，PCA计数器的值与模块的比较/捕获寄存器的值匹配将是CEXn脚翻转
                //PWM1:脉宽调节输出模式，1时，使能CEXn脚用做PWM输出
                //EECF1:使能CCFn中断，使能寄存器CCON中的捕获/比较标志CCFn，用来产生中断
																																						                             
	/***启动PCA阵列计数***/
	CR=1;//CR=1,启动PCA阵列计数。  看151页，其中和CMOD的CIDL位有关，又和IDLE有关，看183页。
    /***将PWM输出端设置在P4口***/
    /***40脚STC单片机不能设置***/
	//AUXR1=0xc0;//PWM0从P1.3切换到P4.2
                 //PWM1从P1.4切换到P4.3							                                                                                                                                                                                                            

}

void PWM_Set(unsigned char PWM0_DATA,unsigned char PWM1_DATA)
{
        CCAP0L=PWM0_DATA;//装入比较初值
	      CCAP0H=PWM0_DATA;
        CCAP1L=PWM1_DATA;//装入比较初值
	      CCAP1H=PWM1_DATA;
}

//选择FOSC/6作为时钟源,11.0592MHZ晶振时，PWM频率=11059200/6/256=7.2Khz
//一个周期=1/7.2K= 0.1388ms
//一个周期分成256份=0.1388ms/256=0.0005421875ms

//由于当CL的值等于或大于｛EPCnL，CCAPnL｝时，输出为高。下面设置CL为0，所以：
//举例PWM0计算方法（PWM1也一样）：
//高电平脉冲时间等于低电平脉冲的时候：CCAP0L=127;CCAP0H=127;  高低电平的时间128*0.0005421875ms=0.0694ms，一个周期约为0.0694ms X 2 = 0.1388ms
//高电平脉冲时间计算：（256-CCAP0L）X 0.0005421875ms
//低电平脉冲时间计算：CCAP0L X 0.0005421875ms
