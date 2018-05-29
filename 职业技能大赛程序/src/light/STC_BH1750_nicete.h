#ifndef  _STC_BH1750_NICETE_H_
#define  _STC_BH1750_NICETE_H_

#include "commom.h"

void Init_BH1750();
void Multiple_read_BH1750(void);
void Single_Write_BH1750(uchar REG_Address);

extern uchar BUF[3];
#endif