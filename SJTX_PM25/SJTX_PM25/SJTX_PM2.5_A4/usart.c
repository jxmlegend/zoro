/* ****************************************************************
** 功能描述: 串行通讯程序
*************************************************************** */
#include	<p18cxxx.h>

/* ****************************************************************
** 函 数 名: sciinitial()
** 功能描述: 232串行通讯初始化子程序，设置低优先级中断接收，使能232收和发
*************************************************************** */
void	sciinitial()
{
//	PMD0bits.UART1MD = 0;  //使能寄存器
//	BAUDCON1bits.TXCKP = 1; //空闲状态下保持低电平。
//    BAUDCON1bits.RXDTP = 1; //接收电平反向。
//    BAUDCON1bits.ABDEN = 0;      //禁止波特率测量
	TXSTA1=0x04;			// 选择异步高速方式传输8位数据
	RCSTA1=0x90;			// 允许串行口工作使能
	TRISC=TRISC|0X80;	// PIC18F458:将RC7(RX)设置为输入方式
	TRISC=TRISC&0Xbf;	// RC6(TX)设置为输出
	BAUDCON1 = 0x08;
	SPBRGH1 = 0;
	SPBRG1 = 207;		// 8M晶振时波特率为207 使能16位之后
	
//	SPBRG=51;			
	PIR1=0x00;			// 清中断标志
	PIE1=PIE1|0x20;		/* 允许串行通讯接口接收中断使能 */
	IPR1bits.RC1IP=1;				// 设置SCI接收中断为低优先级中断 
	RCSTA1bits.CREN=1;				// 允许串口接收数据
	TXSTA1bits.TXEN=1;				// 允许串口发送数据

//	TXSTA1=0x04;			// 选择异步高速方式传输8位数据
//	RCSTA1=0x90;			// 允许串行口工作使能
//	TRISC=TRISC|0X80;	// PIC18F458:将RC7(RX)设置为输入方式
//	TRISC=TRISC&0Xbf;	// RC6(TX)设置为输出
//	BAUDCON1 = 0x08;
//	SPBRGH1 = 0;
//	SPBRG1 = 207;
//	PIR1 &= 0xcf;
//	PIE1 |= 0x30;
//	IPR1 &= 0b11001111;
//	RCSTA1bits.CREN=1;				// 允许串口接收数据
}

void sciinitial2(void)//串口2初始化
{
	TXSTA2=0x04;			// 选择异步高速方式传输8位数据
	RCSTA2=0x90;			// 允许串行口工作使能
	TRISB=TRISB|0X80;	// PIC18F458:将RB7(RX)设置为输入方式
	TRISB=TRISB&0Xbf;	// RB6(TX)设置为输出
	BAUDCON2 = 0x08;
	SPBRGH2 = 0;
	SPBRG2 = 207;		// 8M晶振时波特率为207 使能16位之后
	
//	SPBRG=51;			
	PIR3=0x00;			// 清中断标志
	PIE3=PIE3|0x20;		/* 允许串行通讯接口接收中断使能 */
	IPR3bits.RC2IP=1;				// 设置SCI接收中断为低优先级中断 
	RCSTA2bits.CREN=1;				// 允许串口接收数据
	TXSTA2bits.TXEN=1;				// 允许串口发送数据
}