#ifndef __SIGNAL_H
#define __SIGNAL_H

#include "stm32f10x.h"


#define DAC_DHR12RD_Address     0x40007420
#define DAC_DHR8R1_Address      0x40007410
#define DAC_DHR12R1_Address		0x40007408

/* DMA 传输数据长度 */
enum
{   
   DMA_BufferSize_36   = 36,
	DMA_BufferSize_50   = 50,
	DMA_BufferSize_100  = 100,
};
/* 波形类型 */
enum
{   
    WT_RECT  = 0,	/* 矩形波 */
	WT_SIN   = 1,	/* 正弦波 */
	WT_TRIA	 = 2,	/* 三角波 */
	WT_SAW   = 3,   /* 锯齿波 */
	WT_NOISE = 4,   /* 噪声   */
};
/* 正弦波 */
typedef struct
{
  uint32_t Frequency;     //频率
  uint16_t Amplitude;     //幅值
}SIN_T;

/* 三角波 */
typedef struct
{
  uint16_t Frequency;     //频率
  uint16_t Amplitude;     //幅值
}TRIA_T;

/* 锯齿波 */
typedef struct
{
  uint32_t Frequency;     //频率
  uint16_t Amplitude;     //幅值
}SAW_T;

/* 矩形波 */
typedef struct
{
  uint32_t Frequency;     //频率
  uint16_t Amplitude;     //幅值
  uint8_t  DutyCycle;     //占空比
}RECT_T;

/* 噪声 */
typedef struct
{
  uint16_t Frequency;     //频率
  uint16_t Amplitude;     //幅值
}NOISE_T;

extern SIN_T    g_Sine;
extern RECT_T   g_Rect;
extern SAW_T    g_Saw;
extern NOISE_T  g_Noise;
extern TRIA_T   g_Tria;


void InitWaveMaker(void);
void SetupWaveType(uint8_t _ucType,uint32_t _ulFrequency,uint16_t _usAmplitude,	
                    uint8_t _ucDutyCycle);
#endif
