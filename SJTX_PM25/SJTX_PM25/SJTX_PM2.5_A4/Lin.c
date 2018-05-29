
#include "LIN.h"

LIN_Msg  _SlaveSendMsg;
LIN_Frame SlaveFrame_Receive;


extern INT8U SCIDRL;
extern INT8U SCISR1_RDRF;//����2���ձ�־
extern INT8U recdata[32];
/****************************************
�������ƣ� void LIN_Init( US usBaud)
�������أ� LIN ��ʼ�� ����
��ʽ������ usBaud  -- ����������ֵ
����ֵ��
******************************************/
//void LIN_Init(US usBaud)
//{
//    SCISR2 &= 0x7f ;                // ����AMAP 
//    SCIBD = (US)( (SystemBusClock/16)/usBaud );   //����SCI������ 
//    SCICR1 = 0x00;        //����SCI0Ϊ����ģʽ����λ����λ������żУ��
//    SCICR2 = 0x2c;        //������պͷ������ݣ���������жϹ��� 
//    SCISR2 |=(1<<2);        //����BRK13 13λ�����     
///*
//    SCISR2 &= 0x7f ;                // ����AMAP 
//    SCIBD = (US)( (SystemBusClock/16)/usBaud );   //����SCI������ 
//    SCICR1 = 0x00;        //����SCI0Ϊ����ģʽ����λ����λ������żУ��
//    SCICR1 = 0x04;
//    SCICR2 = 0x2c;        //������պͷ������ݣ���������жϹ���
//    SCICR1|=(1<<2); 
//    SCISR2|=(1<<2);        //����BRK13 13λ�����
//*/     
//}
    
/*************************************************
�������ƣ� void LIN_SendBreak(void)
�������ܣ� ����ͬ�������
��ʽ������ 
����ֵ��   
***************************************************/
INT8U LIN_SendBreak(void)
{
    while(!TXSTA2bits.TRMT);         //�ȴ��������ݼĴ�������������Ϊ��
    TXSTA2bits.SENDB=1;                   //���д����͵���ֹ�ַ�
    while(!TXSTA2bits.SENDB);             //�ȴ�����ַ��������
//    TXREG2=0x00;
    return TRUE;
}

/*******************************************
�������ƣ� void_LIN_SendChar( UC ucCh )
�������ܣ� ����һ���ֽں���
��ʽ������ �跢�͵��ֽ�
����ֵ��   
*********************************************/
INT8U LIN_SendChar(INT8U ucCh)
{
    while(!TXSTA2bits.TRMT);  //�ȴ��������ݼĴ�������������Ϊ��
    TXREG2 = ucCh;
    while(!TXSTA2bits.TRMT);  //�ȴ��������ݼĴ�������������Ϊ��
    return TRUE;
}

/***************************************
�������ƣ�UC LIN_CalcParity( UC ucId )
�������ܣ���żУ��
��ʽ������ucId 
����ֵ��
****************************************/ 
INT8U LIN_CalcParity(INT8U id)
{
    INT8U parity, p0,p1;
    parity=id; 
    p0=(BIT(parity,0)^BIT(parity,1)^BIT(parity,2)^BIT(parity,4))<<6;     //żУ��λ
    p1=(!(BIT(parity,1)^BIT(parity,3)^BIT(parity,4)^BIT(parity,5)))<<7;  //��У��λ
    parity|=(p0|p1);
    return parity;
}

/**********************************************************
�������ƣ� UC LINSend_CalecChecksum( UC id, UC *data, UC length)
�������ܣ� ����У���  (���ͷ� )
��ʽ������ id -- ����ID ��
           *data -- �跢�͵�����
           length -- �����ֽڸ���
����ֵ��   ����У���
********************************************************/
INT8U LINSend_CalcChecksum(INT8U id,INT8U *data,INT8U length)
{
    INT8U i;
    INT16U check_sum = 0;
    //0x3C 0x3D ʹ�ñ�׼��У��
    if(id != 0x3c && id != 0x7d)  //ʹ����ǿ��У��
    {
        check_sum  = id ;
    } 
    else //ʹ�ñ�׼У��
    {
        check_sum = 0 ;
    }
    for (i = 0; i < length; i++) 
    {    
        check_sum += *(data++);

        if (check_sum > 0xFF)      //��λ
        {
            check_sum -= 0xFF;
        }
    }
    return (~check_sum);  //ȡ��
}
/*********************************************************************************
�������ƣ� US LINRec_CalecChecksum( UC id, UC *data, UC length,US checksum_send)
�������ܣ� ����У���  (���շ� )
��ʽ������ id -- ����ID ��
           *data -- ���͵�����
           length -- �����ֽڸ���
           checksum_send  -- ���ͷ�У���
����ֵ��   TRUE - �ɹ�
           FALSE - ʧ��
***********************************************************************************/
INT8U LINRec_CalecChecksum( INT8U id, INT8U *data, INT8U length,INT16U checksum_send)
{
    INT8U i;
    INT16U check_sum = 0;
    INT16U checkrec_sum =0;
    //0x3C 0x3D ʹ�ñ�׼��У��
    if(id != 0x3c && id != 0x7d)  //ʹ����ǿ��У��
    {
        checkrec_sum  = id ;
    } 
    else //ʹ�ñ�׼У��
    {
        checkrec_sum = 0 ;
    }
    for (i = 0; i < length; i++) 
    {    
        checkrec_sum += *(data++);

        if (checkrec_sum > 0xFF)      //��λ
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
�������ƣ� UC LINMaster_Send_Msg( LIN_Msg *SengMsg )
�������ܣ� LIN ������������
��ʽ������ *SengMsg  ---  ��Ҫ���͵�����
           ucAck  -  0 ��������Ӧ��
           ucAck  -  1 ������Ӧ��
����ֵ��   TRUE  --  ���ͳɹ�
           FALSE --  ����ʧ��
******************************************************/
INT8U LINMaster_Send_Msg(LIN_Msg *SendMsg, INT8U ucAck)
{
    INT8U pid ,i ,check_sum;
    if(SendMsg->ID == 0xff||SendMsg->ID == 0x00)
    return (FALSE);
    //���ͼ��ͬ����
    if( !LIN_SendBreak() )
        return FALSE;

    //����ͬ����
    if( !LIN_SendChar(0x55))
        return FALSE;

    //����PID
    pid = LIN_CalcParity(SendMsg->ID); 
    //����PID
    if( !LIN_SendChar(pid) )
        return FALSE;
    if( ucAck )
    {
        //�������ݳ�
        for(i=0; i <SendMsg->length ; i++)    //SendMsg->length
        {
            // �������ݳ�
            if ( !LIN_SendChar(SendMsg->Data[i]))    //SendMsg->Data[i]
                    return FALSE;
        }
        //����У���               SendMsg->ID  &SendMsg->Data SendMsg->length
        check_sum = LINSend_CalcChecksum(pid,SendMsg->Data,SendMsg->length);
        if ( !LIN_SendChar(check_sum) )
            return FALSE;   
    }

    return(TRUE);
}


/************************************************
�������ƣ� UC LINSlave_Send_Msg(LIN_Msg *SendMsg)
�������ܣ� LIN �ӻ���������
��ʽ������ *SengMsg  ---  ��Ҫ���͵�����
����ֵ��   TRUE -- ���ͳɹ�
           FALSE --   ����ʧ��
**************************************************/
INT8U LINSlave_Send_Msg(LIN_Msg *SendMsg)
{
    INT8U i;
    INT8U check_sum;
    INT8U pid;
    
    //����PID
    pid = LIN_CalcParity(SendMsg->ID); 
    //�������ݳ�
    for(i=0; i <SendMsg->length ; i++)    //SendMsg->length
    {
        // �������ݳ�
        if(!LIN_SendChar(SendMsg->Data[i]))    //SendMsg->Data[i]
                return FALSE;
    }
    //����У���               SendMsg->ID  &SendMsg->Data SendMsg->length
    check_sum = LINSend_CalcChecksum(pid,SendMsg->Data,SendMsg->length);
    if( !LIN_SendChar(check_sum) )
            return FALSE;

    return  TRUE;
}


/**************************************************
�������ƣ�UC LIN_GetChar (void)
�������ܣ�LIN ����һ���ֽں���
��ʽ������
����ֵ��  TRUE  -- ���ճɹ�
          FALSE -- ����ʧ��
***************************************************/
INT8U LIN_GetChar(LIN_Frame *Frame_Receive)
{
     static INT8U ch;
    // LIN����ͨ��״̬
    switch(Frame_Receive->State)
    {
        case IDLE:      //����״̬
                        if(SCISR1_RDRF )
                        return(FALSE);  
                        if(SCIDRL)
                        return(FALSE);
                        break;
        case _BREAK:
                        if(!(SCISR1_RDRF))//�жϴ��ڽ���
                        return(FALSE); 
                        if(SCIDRL!=0x55)
                        return(FALSE);  
                        SCISR1_RDRF=0;
                        break; 
        case SYNCH:               /* ��ȡ ID */
                        if(!(SCISR1_RDRF))//�жϴ��ڽ���
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
                        if(!(SCISR1_RDRF))//˵�����ڽ��յ�����
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
�������ƣ�UC LinSlave_Msg_Handle(void)
��������: �ӻ��������ݺ���
��ʽ������
����ֵ��  TRUE  --  �ɹ�
          FALSE --  ʧ��
*****************************************************/
INT8U LINSlave_Msg_Handle(void)
{
    if(SlaveFrame_Receive.State == PROTECTED_IDENTIFIER)
    {  
        /* �ӻ��������� */
        if( (SlaveFrame_Receive.PID == LIN_CalcParity(AC_LIN_ID) ) ) 
        {
            LinFillSendData(); /* װ������� */
            if( LINSlave_Send_Msg(&_SlaveSendMsg) )
                return TRUE;
            else
                return FALSE;
        }
    }
    if(SlaveFrame_Receive.State == CHECKSUM) 
    {
        /* �ӻ��������� */
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
�������ƣ�void LinFillSendData(void)
��������:  װ������� 
��ʽ������
����ֵ��  
*****************************************************/
void LinFillSendData(void)
{
    _SlaveSendMsg.ID=0x00;
    _SlaveSendMsg.Data[0]=0x01;
    _SlaveSendMsg.Data[1]=0x02;
    _SlaveSendMsg.length=2;
}
/***************************************************
�������ƣ�void LinResolveRevData(void)
��������: �������� 
��ʽ������
����ֵ��  
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

