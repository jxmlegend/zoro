#include "stm32f10x.h"
#include <stdio.h>
#include "stdlib.h"
#include "SIGNAL.h"
#include <math.h>

SIN_T    g_Sine;	     /*正弦波*/
RECT_T   g_Rect;	     /*矩形波*/
SAW_T    g_Saw;		     /*锯齿波*/
NOISE_T  g_Noise;	     /*噪声  */
TRIA_T   g_Tria;	     /*三角波*/

uint16_t g_Wave[100];

/*******************************************************************************
*	函数名: InitWaveMaker
*	输  入:	无
*	输  出:	无
*	功能说明：初始化波形发生器。
*******************************************************************************/
void InitWaveMaker(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

		/*初始化DAC 通道1 */	
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;                  					 
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	    
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	 /* 配置PA4为DAC_OUT1  */		
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}
/*
*********************************************************************************************************
*	函 数 名: InitWaveFreq(uint32_t _usFrequency, uint8_t _ucDMABuffuerSize)
*	功能说明: 用于初始化各种波形的产生
*	形    参：_usFrequency     表示要产生波形的触发频率，
*			 _ucDMABuffuerSize 表示DMA要传输的数据数。
*	返 回 值: 无
*********************************************************************************************************
*/
void InitWaveFreq(uint32_t _usFrequency, uint8_t _ucDMABuffuerSize)
{
	DMA_InitTypeDef DMA_InitStructure;

	/*初始化定时器*/	   														  
	TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update);  
	TIM_SetAutoreload(TIM6,  _usFrequency);	  
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	/*初始化DMA */	
	DMA_DeInit(DMA2_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_Wave;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						     
	DMA_InitStructure.DMA_BufferSize = _ucDMABuffuerSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						    
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low ;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	DMA_Cmd(DMA2_Channel3, ENABLE);			
}
/*
*********************************************************************************************************
*	函 数 名: SetupWaveType
*	功能说明: 用于建立产生各种波形
*	形    参：_ucType		表示要产生的波形的类型
			  _ulFrequency	表示波形的触发频率
			  _usAmplitude	表示波形的幅值
			  _ucDutyCycle	表示矩形波的占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void SetupWaveType(uint8_t _ucType,uint32_t _ulFrequency,uint16_t _usAmplitude,	
                    uint8_t _ucDutyCycle)
{
    uint8_t i;

    TIM_Cmd(TIM6, DISABLE); 	
	DAC_Cmd(DAC_Channel_1, DISABLE);		 		 	
	switch (_ucType)
	{
		case WT_RECT:					      /* 方波 */
			InitWaveFreq(720000 / _ulFrequency, DMA_BufferSize_100);  /*要先初始化这个*/
			for (i = 0; i < _ucDutyCycle; i++)
			{
				g_Wave[i] = _usAmplitude*100;//还需进行补偿
			}
			for (i = _ucDutyCycle; i < 100; i++)
			{
				g_Wave[i] = 0;
			}
			break; 
		case WT_SIN:						/* 正弦波 */
		    InitWaveFreq(720000 / _ulFrequency,DMA_BufferSize_100);			
			for (i = 0; i < DMA_BufferSize_100; i++)
			{
			  //g_Wave[i] = 1024+_usAmplitude*sin(2*3.1415926f*99*i/100); 
				g_Wave[i] = 2048+ _usAmplitude*60.61*sin(2*3.1415926f*99*i/100); //最大值2000幅值
			}
			break;
		case WT_SAW:					    /* 锯齿波 */
		   	InitWaveFreq(720000 / _ulFrequency,DMA_BufferSize_100);			
			for (i = 0; i < DMA_BufferSize_100; i++)
			{
			  g_Wave[i] = 100 + i*_usAmplitude*2.4; //79
			}
			break;
		case WT_TRIA:					     /* 三角波*/
			InitWaveFreq(720000 / _ulFrequency,DMA_BufferSize_100);			
			for (i = 0; i < DMA_BufferSize_100; i++)
			{
			  g_Wave[i] = 100 + abs(50-i)*_usAmplitude*2.4; //79
			}
			break;
		case WT_NOISE:					     /*噪声 */
			InitWaveFreq(720000 / _ulFrequency,DMA_BufferSize_100);			
			for (i = 0; i < DMA_BufferSize_100; i++)
			{
			  g_Wave[i] = rand()%_usAmplitude; 
			}
			break;  
		default : break;
	}
	DAC_Cmd(DAC_Channel_1, ENABLE);		 	 
	TIM_Cmd(TIM6, ENABLE);
}
