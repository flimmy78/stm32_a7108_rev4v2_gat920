#ifndef __CALCULATION_AVGOCCUPANCY_H
#define   __CALCULATION_AVGOCCUPANCY_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"


void CALCULATION_GetAvgOccupancy(u8 *buf);									//���ݵشŷ��͵����ݰ�����ƽ��ռ��ʱ��
u16  CALCULATION_ReadAvgOccupancy(u16 outputid);								//��ȡ�ó���ƽ��ռ��ʱ��ֵ
u32  CALCULATION_ReadAvgOccupancyExtend(u16 outputid);							//��ȡ�ó���ƽ��ռ��ʱ��ֵ(����֧������Ϊ4�ֽ�int)

#endif
