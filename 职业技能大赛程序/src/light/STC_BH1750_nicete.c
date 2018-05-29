#include "commom.h"
#include "STC_BH1750_nicete.h"

#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8

uchar BUF[3];

/**************************************
延时5微秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数，注意时钟过快时需要修改
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 10;
	while (--i);
}

/**************************************
延时5毫秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 54;
	j = 198;
	do
	{
		while (--j);
	} while (--i);
}
/**************************************
起始信号
**************************************/
void BH1750_Start()
{
    BH1750_SDA = 1;                    //拉高数据线
    BH1750_SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    BH1750_SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    BH1750_SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void BH1750_Stop()
{
    BH1750_SDA= 0;                    //拉低数据线
    BH1750_SCL= 1;                    //拉高时钟线
    Delay5us();                 //延时
    BH1750_SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(bit ack)
{
    BH1750_SDA = ack;                  //写应答信号
    BH1750_SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    BH1750_SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit BH1750_RecvACK()
{
    BH1750_SCL= 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = BH1750_SDA;                   //读应答信号
    BH1750_SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        BH1750_SDA = CY;               //送数据口
        BH1750_SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        BH1750_SCL= 0;                //拉低时钟线
        Delay5us();             //延时
    }
    BH1750_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE BH1750_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    BH1750_SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        BH1750_SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= BH1750_SDA;             //读数据               
        BH1750_SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//*********************************

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，
  //  BH1750_SendByte(REG_data);       //内部寄存器数据，
    BH1750_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
/*
uchar Single_Read_BH1750(uchar REG_Address)
{  uchar REG_data;
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    BH1750_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BH1750_RecvByte();              //读出寄存器数据
	BH1750_SendACK(1);   
	BH1750_Stop();                           //停止信号
    return REG_data; 
}
*/
//*********************************************************
//
//连续读出BH1750内部数据
//
//*********************************************************
void Multiple_read_BH1750(void)
{   uchar i;	
    BH1750_Start();                          //????
    BH1750_SendByte(SlaveAddress+1);         //??????+???
	
	 for (i=0; i<3; i++)                      //????2?????,???BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]??0x32??????
        if (i == 3)
        {

           BH1750_SendACK(1);                //?????????NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //??ACK
       }
   }

    BH1750_Stop();                          //????
    Delay5ms();
}


//初始化BH1750，根据需要请参考pdf进行修改****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  

}