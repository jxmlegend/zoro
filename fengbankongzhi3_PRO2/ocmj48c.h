#include<msp430f5529.h>
#include<intrinsics.h>

#define uchar unsigned char
#define uint  unsigned int

#define LCD1OUT  P4OUT
#define LCD2OUT  P7OUT
#define LCD3OUT  P8OUT
#define LCD1DIR  P4DIR
#define LCD2DIR  P7DIR
#define LCD3DIR  P8DIR

#define SID  BIT0      //P4.0
#define SCLK BIT2      //P8.2
#define CS   BIT3      //P4.3
#define LCD_PSB BIT4   //P7.4
#define LCD_RST BIT1   //P8.1

extern void disp_chars(uchar x,uchar y,uchar *string);
extern void lcd_delay(uint xms);
extern void Write_Lcd(uchar cmd,uchar dat);
extern void lcd_init();
extern void delayms(uint xms);
extern void lcd_clr();