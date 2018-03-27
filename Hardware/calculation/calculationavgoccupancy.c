/**
  *********************************************************************************************************
  * @file    calculationavgoccupancy.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   ƽ��ռ��ʱ�����
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  ���ݵشŷ��͵����ݰ�����ƽ��ռ��ʱ��							(�ⲿ����)
  *			2.  ��ȡ�ó���ƽ��ռ��ʱ��ֵ										(�ⲿ����)
  *
  *********************************************************************************************************
  */

#include "calculationavgoccupancy.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetAvgOccupancy(u8 *buf)
 @Description			���ݵشŷ��͵����ݰ�����ƽ��ռ��ʱ��
 @Input				*buf				: ���ؽ��յ��شŷ��͵����ݰ�
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetAvgOccupancy(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//����յ��������복���
	if ( (phead->type == DATATYPE_CARIN_WITH_MAGVALUE) || (phead->type == DATATYPE_CARIN_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];									//��ȡ�شŷ������ݰ��еĳ�������
		
		for (i = 0; i < OUTPUT_MAX; i++) {												//����ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {					//ƥ��ͬID
				
				CalculationDataPacket[i].Occupancy_CarNumState = carnumval;					//���복������ͬ��ƥ��
				CalculationDataPacket[i].Occupancy_CarState = 1;							//�õشų������־λ��1
				CalculationDataPacket[i].Occupancy_CarUseTime = 0;						//�峵����ռ��ʱ���ۼ���
				
			}
		}
	}
	//����յ������뿪�����
	else if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];									//��ȡ�شŷ������ݰ��еĳ�������
		
		for (i = 0; i < OUTPUT_MAX; i++) {												//����ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {					//ƥ��ͬID
				
				CalculationDataPacket[i].Occupancy_CarState = 0;							//�õشų������־λ��0
				//�õشŴ˴�ռ��ʱ��
				if ( (CalculationDataPacket[i].Occupancy_CarUseTime != 0) &&				//��ռ��ʱ��
					(carnumval == CalculationDataPacket[i].Occupancy_CarNumState) )			//ͬ��ƥ��
				{
					CalculationDataPacket[i].Occupancy_CarSumTime += CalculationDataPacket[i].Occupancy_CarUseTime;	//�õش�ռ��ʱ���ۼ�
					CalculationDataPacket[i].Occupancy_CarSumNum += 1;										//�õش�ռ�д����ۼ�
				}
				CalculationDataPacket[i].Occupancy_CarUseTime = 0;						//�峵����ռ��ʱ���ۼ���
				
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadAvgOccupancy(u16 outputid)
 @Description			��ȡ�ó���ƽ��ռ��ʱ��ֵ
 @Input				u16   outputid		: 		����ID��
 @Return				u16   AvgOccupancy	: 		�ó���AvgOccupancyֵ
**********************************************************************************************************/
u16 CALCULATION_ReadAvgOccupancy(u16 outputid)
{
	u8 i = 0;
	u16 avgoccupancyval = 0;
	u32 avgoccupancyvaltmp = 0;
	u32 occupancycarsumtime = 0;
	u16 occupancycarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			
			occupancycarsumtime = CalculationDataPacket[i].Occupancy_CarSumTime;				//��ȡ��ʱ����ۼ�ռ��ʱ��
			CalculationDataPacket[i].Occupancy_CarSumTime = 0;
			occupancycarsumnum = CalculationDataPacket[i].Occupancy_CarSumNum;				//��ȡ��ʱ����ۼ�ռ�д���
			CalculationDataPacket[i].Occupancy_CarSumNum = 0;
			
		}
	}
	
	if (occupancycarsumnum != 0) {
		avgoccupancyvaltmp = occupancycarsumtime / occupancycarsumnum;
		if (avgoccupancyvaltmp >= 65535) {
			avgoccupancyval = 65535;
		}
		else {
			avgoccupancyval = avgoccupancyvaltmp;
		}
	}
	
	return avgoccupancyval;
}

/**********************************************************************************************************
 @Function			u32 CALCULATION_ReadAvgOccupancyExtend(u16 outputid)
 @Description			��ȡ�ó���ƽ��ռ��ʱ��ֵ(����֧������Ϊ4�ֽ�int)
 @Input				u16   outputid		: 		����ID��
 @Return				u16   AvgOccupancy	: 		�ó���AvgOccupancyֵ
**********************************************************************************************************/
u32 CALCULATION_ReadAvgOccupancyExtend(u16 outputid)
{
	u8 i = 0;
	u32 avgoccupancyval = 0;
	u32 avgoccupancyvaltmp = 0;
	u32 occupancycarsumtime = 0;
	u16 occupancycarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			
			occupancycarsumtime = CalculationDataPacket[i].Occupancy_CarSumTime;				//��ȡ��ʱ����ۼ�ռ��ʱ��
			CalculationDataPacket[i].Occupancy_CarSumTime = 0;
			occupancycarsumnum = CalculationDataPacket[i].Occupancy_CarSumNum;				//��ȡ��ʱ����ۼ�ռ�д���
			CalculationDataPacket[i].Occupancy_CarSumNum = 0;
			
		}
	}
	
	if (occupancycarsumnum != 0) {
		avgoccupancyvaltmp = occupancycarsumtime / occupancycarsumnum;
		if (avgoccupancyvaltmp >= 2147483647) {
			avgoccupancyval = 2147483647;
		}
		else {
			avgoccupancyval = avgoccupancyvaltmp;
		}
	}
	
	return avgoccupancyval;
}

/********************************************** END OF FLEE **********************************************/
