/*-------------------------------------------------------------------------------
文件名称：main.c
文件描述：通过串口1，打印ADC1电压值（PC1）
硬件平台：尼莫M3S开发板
编写整理：shifang
固件库  ：V3.5
技术论坛：www.doflye.net
备    注：通过简单修改可以移植到其他开发板，部分资料来源于网络。
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "delay.h"
#include "pwm.h"
#include "usart.h"
#include "adc.h"

// ADC1转换的电压值通过DMA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值			 
float ADC_ConvertedValueLocal;     
float sw;
int main(void)
{
	u32 flag                                                                                                          ;
  SysTick_Init();//延时初始化
  USART1_Int(9600);
	ADC1_Init();
	pwm_init();
	
  printf("\n\rADC Test: 水位电压：\r");
  while (1)
  {				
		
	//使用printf函数循环发送固定信息
		Delay_ms(20);
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
		flag=ADC_ConvertedValueLocal*1308+200;//1308
		sw=(-1.5)*ADC_ConvertedValueLocal+2.1;//-1.5
		printf("\r\n 采集数值16进制 = 0x%04X \r\n", ADC_ConvertedValue); 
		printf("\r\n 转换后的实际电压值 = %f V \r\n",ADC_ConvertedValueLocal); 
		printf("\r\n 水位 = %f mL \r\n",sw); 
		TIM_SetCompare2(TIM3,flag);//设置TIMx捕获比较2寄存器值
		Delay_ms(500);


  }
}


