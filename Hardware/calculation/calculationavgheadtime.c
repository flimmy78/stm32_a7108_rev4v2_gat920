/**
  *********************************************************************************************************
  * @file    calculationavgheadtime.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   ƽ����ͷʱ�����
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  ���ݵشŷ��͵����ݰ�����ƽ����ͷʱ��							(�ⲿ����)
  *			2.  ��ȡ�ó���ƽ����ͷʱ��										(�ⲿ����)
  *
  *********************************************************************************************************
  */

#include "calculationavgheadtime.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetAvgHeadTime(u8 *buf)
 @Description			���ݵشŷ��͵����ݰ�����ƽ����ͷʱ��(���������)
 @Input				*buf				: ���ؽ��յ��شŷ��͵����ݰ�
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetAvgHeadTime(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	u16 addval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//����յ��������복���
	if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];													//��ȡ�شŷ������ݰ��еĳ�������
		
		for (i = 0; i < OUTPUT_MAX; i++) {																//����ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {									//ƥ��ͬID
				
				//�����������ۼ�
				if (CalculationDataPacket[i].Headtime_CarNumState == 0) {									//δ�յ��������
					CalculationDataPacket[i].Headtime_CarNumState = carnumval;								//����CarNumState
					CalculationDataPacket[i].Headtime_CarSumNum += 1;
				}
				else {																			//���յ��������
					if (carnumval > CalculationDataPacket[i].Headtime_CarNumState) {
						addval = carnumval - CalculationDataPacket[i].Headtime_CarNumState;					//��������
						CalculationDataPacket[i].Headtime_CarSumNum += addval;								//�����ۼ�
						CalculationDataPacket[i].Headtime_CarNumState = carnumval;							//״̬��ֵ
					}
					else if (carnumval < CalculationDataPacket[i].Headtime_CarNumState) {
						addval = (0xffff - CalculationDataPacket[i].Headtime_CarNumState) + carnumval;			//��������
						CalculationDataPacket[i].Headtime_CarSumNum += addval;								//�����ۼ�
						CalculationDataPacket[i].Headtime_CarNumState = carnumval;							//״̬��ֵ
					}
				}
				
				//��ͷʱ������ۼ�
				if (CalculationDataPacket[i].Headtime_CarState == 0) {										//��ʱ��δ����
					CalculationDataPacket[i].Headtime_CarState = 1;										//������ʱ��
					CalculationDataPacket[i].Headtime_CarUseTime = 0;										//���ʱ������
				}
				else {																			//��ʱ���ѿ���
					if (CalculationDataPacket[i].Headtime_CarUseTime != 0) {								//�ۼ�ʱ��
						if (addval > 0) {															//�г�����
							CalculationDataPacket[i].Headtime_CarSumTime += CalculationDataPacket[i].Headtime_CarUseTime;
						}
					}
					CalculationDataPacket[i].Headtime_CarUseTime = 0;										//���ʱ������
				}
				
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadAvgHeadTime(u16 outputid)
 @Description			��ȡ�ó���ƽ����ͷʱ��(ͳ��ʱ�䵽����)
 @Input				u16   outputid		: 		����ID��
 @Return				u16   AvgHeadTime	: 		�ó���AvgHeadTimeֵ
**********************************************************************************************************/
u16 CALCULATION_ReadAvgHeadTime(u16 outputid)
{
	u8 i = 0;
	u16 avgheadtimeval = 0;
	u32 avgheadtimevaltmp = 0;
	u32 headtimecarsumtime = 0;
	u16 headtimecarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//��ȡͳ��ʱ���ڳ������복���ʱ��
		if (outputid == CalculationDataPacket[i].OutputID) {
			headtimecarsumnum = CalculationDataPacket[i].Headtime_CarSumNum;
			CalculationDataPacket[i].Headtime_CarSumNum = 0;
			headtimecarsumtime = CalculationDataPacket[i].Headtime_CarSumTime;
			CalculationDataPacket[i].Headtime_CarSumTime = 0;
		}
	}
	
	if (headtimecarsumnum != 0) {											//��ʱ������г�
		avgheadtimevaltmp = headtimecarsumtime / headtimecarsumnum;
		if (avgheadtimevaltmp >= 65535) {
			avgheadtimeval = 65535;
		}
		else {
			avgheadtimeval = avgheadtimevaltmp;
		}
	}
	else {															//��ʱ������޳�
		avgheadtimeval = 0;
	}
	
	return avgheadtimeval;
}

/**********************************************************************************************************
 @Function			u32 CALCULATION_ReadAvgHeadTimeExtend(u16 outputid)
 @Description			��ȡ�ó���ƽ����ͷʱ��(����֧������Ϊ4�ֽ�int)(ͳ��ʱ�䵽����)
 @Input				u16   outputid		: 		����ID��
 @Return				u16   AvgHeadTime	: 		�ó���AvgHeadTimeֵ
**********************************************************************************************************/
u32 CALCULATION_ReadAvgHeadTimeExtend(u16 outputid)
{
	u8 i = 0;
	u32 avgheadtimeval = 0;
	u32 headtimecarsumtime = 0;
	u32 avgheadtimevaltmp = 0;
	u16 headtimecarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//��ȡͳ��ʱ���ڳ������복���ʱ��
		if (outputid == CalculationDataPacket[i].OutputID) {
			headtimecarsumnum = CalculationDataPacket[i].Headtime_CarSumNum;
			CalculationDataPacket[i].Headtime_CarSumNum = 0;
			headtimecarsumtime = CalculationDataPacket[i].Headtime_CarSumTime;
			CalculationDataPacket[i].Headtime_CarSumTime = 0;
		}
	}
	
	if (headtimecarsumnum != 0) {											//��ʱ������г�
		avgheadtimevaltmp = headtimecarsumtime / headtimecarsumnum;
		if (avgheadtimevaltmp >= 2147483647) {
			avgheadtimeval = 2147483647;
		}
		else {
			avgheadtimeval = avgheadtimevaltmp;
		}
	}
	else {															//��ʱ������޳�
		avgheadtimeval = 0;
	}
	
	return avgheadtimeval;
}

/********************************************** END OF FLEE **********************************************/
