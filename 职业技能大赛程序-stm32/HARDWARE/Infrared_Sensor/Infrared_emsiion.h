#ifndef _INFRARED_EMSIION_H
#define _INFRARED_EMSIION_H

#include "sys.h"


#define control_send PBout(1)	// PB1


  void Infrared_IO_init(void);
  void SendIRdata_BYTE(u8 irdata);
  void SendIRdata(unsigned char g_iraddr1,unsigned char p_irdata);
	
	/*
发送4字节结束码数据
*/
void SendEnddata(unsigned char dat1,unsigned char dat2);

	void Multiple_sendIRdata(u8 count,u8 ir_adress,u8 dat);

#endif

