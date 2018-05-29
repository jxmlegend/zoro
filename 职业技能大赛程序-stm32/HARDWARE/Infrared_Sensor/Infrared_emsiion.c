#include "Infrared_emsiion.h"
#include "delay.h"
void Infrared_IO_init(void)
{
		 GPIO_InitTypeDef  GPIO_InitStructure;
			
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
			
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //LED0-->PB.0 �˿�����
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0
		 GPIO_SetBits(GPIOB,GPIO_Pin_1);						 //PB.0 �����
}

/*************************   ˵ �� *********************************/
/*    ��һ��9ms�ĵ͵�ƽ��4.5ms�ĸߵ�ƽΪ�����룬���32λ�����ƴ��� */
/*    ǰ16λΪ8λ�û��뼰�䷴�룬��16λΪ8λ�Ĳ����뼰�䷴��      */
/*    ������Ϊ0.565ms�����0.56ms������Ϊ1.125ms����ϱ�ʾ"0"��    */
/*    ������Ϊ0.565ms�����1.685ms������Ϊ2.25ms����ϱ�ʾ"1"��    */
/*    ע�⣺����������������ǶԷ�����ȡ���ģ��������0.565ms     */

void SendIRdata_BYTE(u8 irdata)
{
		 u8 i;
		 for(i=0;i<8;i++)
		 {	
			 control_send=0;  //�ȷ���0.56ms��38KHZ���Ⲩ����������0.56ms�ĸߵ�ƽ��
			 delay_us(560);
			//ֹͣ���ͺ����źţ��������еĵ͵�ƽ��
			if(irdata&1)      //�ж����λΪ1����0��   ��λ�ȷ��ͣ���
			 { control_send=1;  //1Ϊ���ƽ��1.693ms
			 delay_us(1693);}
			else {control_send=1;  //0Ϊխ��ƽ��0.56ms
						delay_us(560);}
				 irdata=irdata>>1;     
			}
}
/*
����4�ֽ�����
�������������ݷ��͵�ַ�����͵�ָ��
*/
void SendIRdata(unsigned char g_iraddr1,unsigned char p_irdata)
{
			 //����9ms����ʼ�룬�ߵ�ƽ��38KHZ�ز�
			control_send=0; 
				delay_ms(9);
			 //����4.5ms�Ľ���룬�͵�ƽ��38KHZ�ز�
			control_send=1; 
			delay_us(4500);
			
			//����ʮ��λ��ַ
			SendIRdata_BYTE(g_iraddr1);
			SendIRdata_BYTE(~g_iraddr1);
				//����8λ����
			SendIRdata_BYTE(p_irdata);
				//����8λ���ݵķ���
			SendIRdata_BYTE(~p_irdata);
			 
			control_send=0; 
			delay_us(560);
			control_send=1; 
}
/*
����4�ֽڽ���������

*/
void SendEnddata(unsigned char dat1,unsigned char dat2)
{
			 //����9ms����ʼ�룬�ߵ�ƽ��38KHZ�ز�
			control_send=0; 
			delay_ms(9);
			 //����4.5ms�Ľ���룬�͵�ƽ��38KHZ�ز�
			control_send=1; 
			delay_us(4500);
			
			//����ʮ��λ��ַ
			SendIRdata_BYTE(dat1);
			SendIRdata_BYTE(dat1);
				//����8λ����
			SendIRdata_BYTE(dat2);
				//����8λ���ݵķ���
			SendIRdata_BYTE(dat2);
			 
			control_send=0; 
			delay_us(560);
			control_send=1; 
}
/*
���ӷ��ʹ�������֤ͨ������
�βΣ� count ����
       ir_adress  �������ݷ��͵�ַ
       dat       ָ��
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
