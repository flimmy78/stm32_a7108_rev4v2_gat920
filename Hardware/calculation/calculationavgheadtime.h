#ifndef __CALCULATION_AVGHEADTIME_H
#define   __CALCULATION_AVGHEADTIME_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"


void CALCULATION_GetAvgHeadTime(u8 *buf);									//���ݵشŷ��͵����ݰ�����ƽ����ͷʱ��
u16  CALCULATION_ReadAvgHeadTime(u16 outputid);								//��ȡ�ó���ƽ����ͷʱ��
u32  CALCULATION_ReadAvgHeadTimeExtend(u16 outputid);							//��ȡ�ó���ƽ����ͷʱ��(����֧������Ϊ4�ֽ�int)

#endif
