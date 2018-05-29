#include "Infrared_emsiion.h"
#include "delay.h"
void Infrared_IO_init(void)
{
		 GPIO_InitTypeDef  GPIO_InitStructure;
			
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
			
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //LED0-->PB.0 端口配置
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
		 GPIO_SetBits(GPIOB,GPIO_Pin_1);						 //PB.0 输出高
}

/*************************   说 明 *********************************/
/*    以一个9ms的低电平和4.5ms的高电平为引导码，后跟32位二进制代码 */
/*    前16位为8位用户码及其反码，后16位为8位的操作码及其反码      */
/*    以脉宽为0.565ms、间隔0.56ms、周期为1.125ms的组合表示"0"；    */
/*    以脉宽为0.565ms、间隔1.685ms、周期为2.25ms的组合表示"1"。    */
/*    注意：接收码的脉宽与间隔是对发射码取反的，即间隔是0.565ms     */

void SendIRdata_BYTE(u8 irdata)
{
		 u8 i;
		 for(i=0;i<8;i++)
		 {	
			 control_send=0;  //先发送0.56ms的38KHZ红外波（即编码中0.56ms的高电平）
			 delay_us(560);
			//停止发送红外信号（即编码中的低电平）
			if(irdata&1)      //判断最低位为1还是0。   低位先发送！！
			 { control_send=1;  //1为宽电平，1.693ms
			 delay_us(1693);}
			else {control_send=1;  //0为窄电平，0.56ms
						delay_us(560);}
				 irdata=irdata>>1;     
			}
}
/*
发送4字节数据
参数：红外数据发送地址，发送的指令
*/
void SendIRdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
			 //发送9ms的起始码，高电平有38KHZ载波
			control_send=0; 
				delay_ms(9);
			 //发送4.5ms的结果码，低电平无38KHZ载波
			control_send=1; 
			delay_us(4500);
			
			//发送十六位地址
			SendIRdata_BYTE(g_iraddr1);
			SendIRdata_BYTE(~g_iraddr1);
				//发送8位数据
			SendIRdata_BYTE(p_irdata);
				//发送8位数据的反码
			SendIRdata_BYTE(~p_irdata);
			 
			control_send=0; 
			delay_us(560);
			control_send=1; 
}
/*
发送4字节结束码数据

*/
void SendEnddata(unsigned char dat1,unsigned char dat2)
{
			 //发送9ms的起始码，高电平有38KHZ载波
			control_send=0; 
			delay_ms(9);
			 //发送4.5ms的结果码，低电平无38KHZ载波
			control_send=1; 
			delay_us(4500);
			
			//发送十六位地址
			SendIRdata_BYTE(dat1);
			SendIRdata_BYTE(dat1);
				//发送8位数据
			SendIRdata_BYTE(dat2);
				//发送8位数据的反码
			SendIRdata_BYTE(dat2);
			 
			control_send=0; 
			delay_us(560);
			control_send=1; 
}
/*
增加发送次数，保证通信质量
形参： count 次数
       ir_adress  红外数据发送地址
       dat       指令
*/
void Multiple_sendIRdata(u8 count,u8 ir_adress,u8 dat)
{
			u8 i;
      for(i=0;i<count;i++)
			{
		    SendIRdata(ir_adress,dat);
			  delay_ms(10);
				SendEnddata(0xff,0xff);
			}

} 
