#ifndef __CALCULATION_VOLUME_H
#define   __CALCULATION_VOLUME_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"

void CALCULATION_GetVolume(u8 *buf);										//���ݵشŷ��͵����ݰ����㳵������
u16  CALCULATION_ReadVolume(u16 outputid);									//��ȡ�ó���������ֵ

#endif
