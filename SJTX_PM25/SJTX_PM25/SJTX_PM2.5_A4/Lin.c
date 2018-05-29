
#include "LIN.h"

LIN_Msg  _SlaveSendMsg;
LIN_Frame SlaveFrame_Receive;


extern INT8U SCIDRL;
extern INT8U SCISR1_RDRF;//串口2接收标志
extern INT8U recdata[32];
/****************************************
函数名称： void LIN_Init( US usBaud)
函数贡呢： LIN 初始化 函数
形式参数： usBaud  -- 波特率设置值
返回值：
******************************************/
//void LIN_Init(US usBaud)
//{
//    SCISR2 &= 0x7f ;                // 设置AMAP 
//    SCIBD = (US)( (SystemBusClock/16)/usBaud );   //设置SCI波特率 
//    SCICR1 = 0x00;        //设置SCI0为正常模式，八位数据位，无奇偶校验
//    SCICR2 = 0x2c;        //允许接收和发送数据，允许接收中断功能 
//    SCISR2 |=(1<<2);        //设置BRK13 13位间隔符     
///*
//    SCISR2 &= 0x7f ;                // 设置AMAP 
//    SCIBD = (US)( (SystemBusClock/16)/usBaud );   //设置SCI波特率 
//    SCICR1 = 0x00;        //设置SCI0为正常模式，八位数据位，无奇偶校验
//    SCICR1 = 0x04;
//    SCICR2 = 0x2c;        //允许接收和发送数据，允许接收中断功能
//    SCICR1|=(1<<2); 
//    SCISR2|=(1<<2);        //设置BRK13 13位间隔符
//*/     
//}
    
/*************************************************
函数名称： void LIN_SendBreak(void)
函数功能： 发送同步间隔段
形式参数： 
返回值：   
***************************************************/
INT8U LIN_SendBreak(void)
{
    while(!TXSTA2bits.TRMT);         //等待发送数据寄存器（缓冲器）为空
    TXSTA2bits.SENDB=1;                   //队列待发送的中止字符
    while(!TXSTA2bits.SENDB);             //等待间隔字符发送完毕
//    TXREG2=0x00;
    return TRUE;
}

/*******************************************
函数名称： void_LIN_SendChar( UC ucCh )
函数功能： 发送一个字节函数
形式参数： 需发送的字节
返回值：   
*********************************************/
INT8U LIN_SendChar(INT8U ucCh)
{
    while(!TXSTA2bits.TRMT);  //等待发送数据寄存器（缓冲器）为空
    TXREG2 = ucCh;
    while(!TXSTA2bits.TRMT);  //等待发送数据寄存器（缓冲器）为空
    return TRUE;
}

/***************************************
函数名称：UC LIN_CalcParity( UC ucId )
函数功能：奇偶校验
形式参数：ucId 
返回值：
****************************************/ 
INT8U LIN_CalcParity(INT8U id)
{
    INT8U parity, p0,p1;
    parity=id; 
    p0=(BIT(parity,0)^BIT(parity,1)^BIT(parity,2)^BIT(parity,4))<<6;     //偶校验位
    p1=(!(BIT(parity,1)^BIT(parity,3)^BIT(parity,4)^BIT(parity,5)))<<7;  //奇校验位
    parity|=(p0|p1);
    return parity;
}

/**********************************************************
函数名称： UC LINSend_CalecChecksum( UC id, UC *data, UC length)
函数功能： 计算校验和  (发送方 )
形式参数： id -- 报文ID 号
           *data -- 需发送的数据
           length -- 发送字节个数
返回值：   返回校验和
********************************************************/
INT8U LINSend_CalcChecksum(INT8U id,INT8U *data,INT8U length)
{
    INT8U i;
    INT16U check_sum = 0;
    //0x3C 0x3D 使用标准型校验
    if(id != 0x3c && id != 0x7d)  //使用增强型校验
    {
        check_sum  = id ;
    } 
    else //使用标准校验
    {
        check_sum = 0 ;
    }
    for (i = 0; i < length; i++) 
    {    
        check_sum += *(data++);

        if (check_sum > 0xFF)      //进位
        {
            check_sum -= 0xFF;
        }
    }
    return (~check_sum);  //取反
}
/*********************************************************************************
函数名称： US LINRec_CalecChecksum( UC id, UC *data, UC length,US checksum_send)
函数功能： 计算校验和  (接收方 )
形式参数： id -- 报文ID 号
           *data -- 发送的数据
           length -- 发送字节个数
           checksum_send  -- 发送方校验和
返回值：   TRUE - 成功
           FALSE - 失败
***********************************************************************************/
INT8U LINRec_CalecChecksum( INT8U id, INT8U *data, INT8U length,INT16U checksum_send)
{
    INT8U i;
    INT16U check_sum = 0;
    INT16U checkrec_sum =0;
    //0x3C 0x3D 使用标准型校验
    if(id != 0x3c && id != 0x7d)  //使用增强型校验
    {
        checkrec_sum  = id ;
    } 
    else //使用标准校验
    {
        checkrec_sum = 0 ;
    }
    for (i = 0; i < length; i++) 
    {    
        checkrec_sum += *(data++);

        if (checkrec_sum > 0xFF)      //进位
        {
            checkrec_sum -= 0xFF;
        }
    }
    check_sum = (checkrec_sum + checksum_send);
    if( check_sum == 0xff)
        return TRUE;
    else
        return FALSE;
}


/***************************************************
函数名称： UC LINMaster_Send_Msg( LIN_Msg *SengMsg )
函数功能： LIN 主机发送数据
形式参数： *SengMsg  ---  需要发送的数据
           ucAck  -  0 主机不带应答
           ucAck  -  1 主机带应答
返回值：   TRUE  --  发送成功
           FALSE --  发送失败
******************************************************/
INT8U LINMaster_Send_Msg(LIN_Msg *SendMsg, INT8U ucAck)
{
    INT8U pid ,i ,check_sum;
    if(SendMsg->ID == 0xff||SendMsg->ID == 0x00)
    return (FALSE);
    //发送间隔同步段
    if( !LIN_SendBreak() )
        return FALSE;

    //发送同步场
    if( !LIN_SendChar(0x55))
        return FALSE;

    //计算PID
    pid = LIN_CalcParity(SendMsg->ID); 
    //发送PID
    if( !LIN_SendChar(pid) )
        return FALSE;
    if( ucAck )
    {
        //发送数据场
        for(i=0; i <SendMsg->length ; i++)    //SendMsg->length
        {
            // 发送数据场
            if ( !LIN_SendChar(SendMsg->Data[i]))    //SendMsg->Data[i]
                    return FALSE;
        }
        //计算校验和               SendMsg->ID  &SendMsg->Data SendMsg->length
        check_sum = LINSend_CalcChecksum(pid,SendMsg->Data,SendMsg->length);
        if ( !LIN_SendChar(check_sum) )
            return FALSE;   
    }

    return(TRUE);
}


/************************************************
函数名称： UC LINSlave_Send_Msg(LIN_Msg *SendMsg)
函数功能： LIN 从机发送数据
形式参数： *SengMsg  ---  需要发送的数据
返回值：   TRUE -- 发送成功
           FALSE --   发送失败
**************************************************/
INT8U LINSlave_Send_Msg(LIN_Msg *SendMsg)
{
    INT8U i;
    INT8U check_sum;
    INT8U pid;
    
    //计算PID
    pid = LIN_CalcParity(SendMsg->ID); 
    //发送数据场
    for(i=0; i <SendMsg->length ; i++)    //SendMsg->length
    {
        // 发送数据场
        if(!LIN_SendChar(SendMsg->Data[i]))    //SendMsg->Data[i]
                return FALSE;
    }
    //计算校验和               SendMsg->ID  &SendMsg->Data SendMsg->length
    check_sum = LINSend_CalcChecksum(pid,SendMsg->Data,SendMsg->length);
    if( !LIN_SendChar(check_sum) )
            return FALSE;

    return  TRUE;
}


/**************************************************
函数名称：UC LIN_GetChar (void)
函数功能：LIN 接收一个字节函数
形式参数：
返回值：  TRUE  -- 接收成功
          FALSE -- 接收失败
***************************************************/
INT8U LIN_GetChar(LIN_Frame *Frame_Receive)
{
     static INT8U ch;
    // LIN接收通道状态
    switch(Frame_Receive->State)
    {
        case IDLE:      //闲置状态
                        if(SCISR1_RDRF )
                        return(FALSE);  
                        if(SCIDRL)
                        return(FALSE);
                        break;
        case _BREAK:
                        if(!(SCISR1_RDRF))//判断串口接收
                        return(FALSE); 
                        if(SCIDRL!=0x55)
                        return(FALSE);  
                        SCISR1_RDRF=0;
                        break; 
        case SYNCH:               /* 获取 ID */
                        if(!(SCISR1_RDRF))//判断串口接收
                        return(FALSE);                         
                        ch = SCIDRL;
                        Frame_Receive->PID = ch;
                        SCISR1_RDRF=0;
                        break;   
        case PROTECTED_IDENTIFIER:
        case DATA0:
        case DATA1:
        case DATA2:
        case DATA3:
        case DATA4:
        case DATA5:
        case DATA6:
                        if(!(SCISR1_RDRF))//说明串口接收到数据
                        return(FALSE); 
                        ch = SCIDRL;
                        Frame_Receive->Data[Frame_Receive->State-PROTECTED_IDENTIFIER] = ch;
                        SCISR1_RDRF=0;
                        break;
        case DATA7:
                        if(!(SCISR1_RDRF))
                        return(FALSE); 
                        ch = SCIDRL;
                        Frame_Receive->Check = ch;
                        SCISR1_RDRF=0;
                        break;
        case CHECKSUM:
                        return(FALSE);  
    }
    Frame_Receive->State+=1;
    return(TRUE);
}



/***************************************************
函数名称：UC LinSlave_Msg_Handle(void)
函数功能: 从机接收数据后处理
形式参数：
返回值：  TRUE  --  成功
          FALSE --  失败
*****************************************************/
INT8U LINSlave_Msg_Handle(void)
{
    if(SlaveFrame_Receive.State == PROTECTED_IDENTIFIER)
    {  
        /* 从机发送数据 */
        if( (SlaveFrame_Receive.PID == LIN_CalcParity(AC_LIN_ID) ) ) 
        {
            LinFillSendData(); /* 装填发送数据 */
            if( LINSlave_Send_Msg(&_SlaveSendMsg) )
                return TRUE;
            else
                return FALSE;
        }
    }
    if(SlaveFrame_Receive.State == CHECKSUM) 
    {
        /* 从机接收数据 */
        if( (SlaveFrame_Receive.PID == LIN_CalcParity(HVAC_LIN_ID))
            &&(LINRec_CalecChecksum(SlaveFrame_Receive.PID,SlaveFrame_Receive.Data,8,SlaveFrame_Receive.Check)))   
        {
            LinResolveRevData();       
        }
        SlaveFrame_Receive.State = IDLE; 
        return TRUE;   
    }
    return FALSE;
     
}
/***************************************************
函数名称：void LinFillSendData(void)
函数功能:  装填发送数据 
形式参数：
返回值：  
*****************************************************/
void LinFillSendData(void)
{
    _SlaveSendMsg.ID=0x00;
    _SlaveSendMsg.Data[0]=0x01;
    _SlaveSendMsg.Data[1]=0x02;
    _SlaveSendMsg.length=2;
}
/***************************************************
函数名称：void LinResolveRevData(void)
函数功能: 接收数据 
形式参数：
返回值：  
*****************************************************/
void LinResolveRevData(void)
{
    if(SlaveFrame_Receive.Data[0]=0x01)
    {
        
    }
    if(SlaveFrame_Receive.Data[1]=0x02)
    {
        
    }
}

