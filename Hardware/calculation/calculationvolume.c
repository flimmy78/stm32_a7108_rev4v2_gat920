/**
  *********************************************************************************************************
  * @file    calculationvolume.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   ����������
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  ���ݵشŷ��͵����ݰ����㳵������								(�ⲿ����)
  *			2.  ��ȡ�ó���������ֵ											(�ⲿ����)
  *
  *********************************************************************************************************
  */

#include "calculationvolume.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetVolume(u8 *buf)
 @Description			���ݵشŷ��͵����ݰ����㳵������
 @Input				*buf				: ���ؽ��յ��شŷ��͵����ݰ�
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetVolume(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	u16 addval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//����յ������뿪�����
	if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) )
	{
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];						//��ȡ�شŷ������ݰ��еĳ�������
		for (i = 0; i < OUTPUT_MAX; i++) {									//����ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {		//ƥ��ͬID
				if (CalculationDataPacket[i].CarNumState == 0) {				//�����յ���������
					CalculationDataPacket[i].CarNumState = carnumval;			//����״ֵ̬
					CalculationDataPacket[i].CarNumProcess += 1;
				}
				else {
					if (carnumval > CalculationDataPacket[i].CarNumState) {
						addval = carnumval - CalculationDataPacket[i].CarNumState;			//��������
						CalculationDataPacket[i].CarNumProcess += addval;					//�����ۼ�
						CalculationDataPacket[i].CarNumState = carnumval;					//״̬��ֵ
					}
					else if (carnumval < CalculationDataPacket[i].CarNumState) {
						addval = (0xffff - CalculationDataPacket[i].CarNumState) + carnumval;	//��������
						CalculationDataPacket[i].CarNumProcess += addval;					//�����ۼ�
						CalculationDataPacket[i].CarNumState = carnumval;					//״̬��ֵ
					}
				}
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadVolume(u16 outputid)
 @Description			��ȡ�ó���������ֵ
 @Input				u16 outputid   : 		����ID��
 @Return				u16 Volume	: 		�ó���Volumeֵ
**********************************************************************************************************/
u16 CALCULATION_ReadVolume(u16 outputid)
{
	u8 i = 0;
	u16 volumeval = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			volumeval = CalculationDataPacket[i].CarNumProcess;
			CalculationDataPacket[i].CarNumProcess = 0;
		}
	}
	
	return volumeval;
}

/********************************************** END OF FLEE **********************************************/
