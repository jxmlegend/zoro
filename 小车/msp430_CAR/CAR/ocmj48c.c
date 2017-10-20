#include<msp430f5529.h>
#include<intrinsics.h>
#include"ocmj48c.h"

//#define comm 0
//#define dat   1
//#define x1    0x80
//#define x2    0x88
//#define y     0x80

//unsigned int img[]="";

//void Write_Lcd(uchar dat_cmd,uchar dat);

void lcd_delay(uint xms)
{
	uchar i;
	uchar j;
	while(xms--)
	{       
                for(j=0;j<10;j++)
		for(i=0;i<2;i++)
		_NOP();
	}
}
void delayms(uint xms)
{
	uint i,j,k;
        for(k=0;k<10;k++)
	for(i=110;i>0;i--)
	for(j=xms;j>0;j--);

}
void Write_Lcd(uchar dat_cmd,uchar dat)	 ///1Ϊд���0Ϊд���ݣ�
{
	 uchar i;
	 uchar j;
	 uchar I_data=0;
	 I_data=dat;
	 lcd_delay(20);
	 if(dat_cmd)
	 {	
		LCD1OUT|=CS;
		LCD3OUT&=~SCLK;
		for(j=0;j<=2;j++)
		{
			if(j==0) I_data=0xf8;
			if(j==1)
			{ 
				I_data=dat;			
	 			I_data&=0xf0;
			}
			if(j==2)
			{
				I_data=dat;		
                                I_data<<=4;
			}
			for(i=0;i<8;i++)
			{
		    	
				if(I_data&0x80) LCD1OUT|=SID;
                                else LCD1OUT&=~SID;
				LCD3OUT&=~SCLK;
                                lcd_delay(1);
				LCD3OUT|=SCLK;
		    	        I_data<<=1;     		
                        }
                }		
            LCD1OUT&=~CS;
            delayms(1);
	 }
	 else 
	 {
	 	LCD1OUT|=CS;
		LCD3OUT&=~SCLK;
		for(j=0;j<=2;j++)
		{
			if(j==0) I_data=0xfa;
			if(j==1)
			{ 
				I_data=dat;			
	 			I_data&=0xf0;
			}
			if(j==2)
			{
				I_data=dat;		
	    		I_data<<=4;
			}
			for(i=0;i<8;i++)
			{
		    
				
				if(I_data&0x80) LCD1OUT|=SID;
                                else LCD1OUT&=~SID;
				LCD3OUT&=~SCLK;
                                lcd_delay(1);
				LCD3OUT|=SCLK;
			        I_data<<=1;	    	 		
			}
	 }
	
	 LCD1OUT&=~CS;
	 delayms(1);///������������ڴ˺�������ʱ�����ж�æ��	
	 }
	
}
void lcd_init()
{
    LCD1DIR|=BIT5+BIT6;
    LCD2DIR|=BIT4;
    LCD3DIR|=BIT1+BIT2;
  
    LCD2OUT&=~LCD_PSB ;         //���ڷ�ʽ    
    LCD3OUT&=~LCD_RST ;		 //Һ����λ
    delayms(3);                   
    LCD3OUT|=LCD_RST;      
    delayms(3);
    Write_Lcd(1,0x34);      //����ָ�����
    delayms(3);
    Write_Lcd(1,0x30);      //����ָ�����
    delayms(3);
    Write_Lcd(1,0x0C);      //��ʾ�����ع��
    delayms(3);
    Write_Lcd(1,0x01);      //���LCD����ʾ����
    delayms(3);
    Write_Lcd(1,0x01);      //���LCD����ʾ����
    delayms(3);
}
void disp_chars(uchar x,uchar y,uchar *str) //x:0~7  y:0~3  *str="�L���I�WԺwww.cjxy.edu.cn"
{
   	 uchar add=0;
	 switch(y)
	 {
	 	case 0:add=0x80+x;break;
		case 1:add=0x90+x;break;
		case 2:add=0x88+x;break;
		case 3:add=0x98+x;break;
	 }
	 Write_Lcd(1,add);
	 while(*str!='\0')
	 {
	 	Write_Lcd(0,*str++);
	 }

} 

////=================================================================
////������:void send_img(unsigned code *img)
////��������: ��ʾͼ��
////��ڲ���:uchar code *img     
////���ڲ�������
////ע�⣺��ʾͼ��ʱ��д��һ�����ݣ�������� 
////================================================================= 
//void send_disp (uchar code,uint *img)
//{
//uchar i,j;
//for(j=0;j<32;j++)      //�ϰ���д��ͼ������
//{
//    for(i=0;i<8;i++)
//    {
//      Write_Lcd (comm,0x34); //8λ���ƶ˿ڣ�ѡ������ָ�
//      Write_Lcd (comm,y+j); //ѡ��ͼ����Y������
//      Write_Lcd (comm,x1+i); //ѡ��ͼ����X������
//      Write_Lcd (comm,0x30); //ѡ�����ָ�
//      Write_Lcd (dat,img[j*16+i*2]); //дͼ������
//      Write_Lcd (dat,img[j*16+i*2+1]);
//    }
//}
//for(j=32;j<64;j++)    //�°���д��ͼ������
//{
//    for(i=0;i<8;i++)
//    {
//      Write_Lcd (comm,0x34);
//      Write_Lcd (comm,y+j-32);
//      Write_Lcd (comm,x2+i);
//      Write_Lcd (comm,0x30);
//      Write_Lcd (dat,img[j*16+i*2]);
//      Write_Lcd (dat,img[j*16+i*2+1]);
//    }
//}
//Write_Lcd (comm,0x36);   //д�����ݺ�ѡ��ͼ����ʾ
//}


