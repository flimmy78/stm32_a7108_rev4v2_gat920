/**
  *********************************************************************************************************
  * @file    gatupload.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  ��ʼ��Upload����
  *			2.  �������Ƿ�����
  *			3.  �������Ƿ�Ϊ��
  *			4.  ��������д�����
  *			5.  �������ݶ�������(����ͷƫ��1)
  *			6.  �������ݶ�������(����ͷ����)
  *			7.  �������ݶ���(����ͷƫ��1)
  *			8.  �����ϴ�����Ӧ��ʱ����
  *			9.  ��ʼ��Statistical����
  *			10. ���Statistical�����Ƿ�����
  *			11. ���Statistical�����Ƿ�Ϊ��
  *			12. ͳ������д�����
  *			13. ͳ�����ݶ�������(����ͷƫ��1)
  *			14. ͳ�����ݶ�������(����ͷ����)
  *			15. ͳ�����ݶ���(����ͷƫ��1)
  *			16. ����ͳ��ʱ�佫ͳ�����ݴ������
  *********************************************************************************************************
  */

#include "gatupload.h"
#include "calculationconfig.h"
#include "string.h"


Gat_UploadQueue			gatUploadQueus;								//�������ݴ洢����
Gat_StatisticalQueue		gatStatisticalQueue;							//ͳ�����ݴ洢����
Gat_StatisticalData			gatStatisticalData;								//�����е���ͳ������
volatile u8 gatOverTime = 0;
volatile u8 gatRepeat = 0;

/**********************************************************************************************************
 @Function			void GAT_InitUploadQueue(void)
 @Description			��ʼ��Upload����
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_InitUploadQueue(void)
{
	gatUploadQueus.front = 0;
	gatUploadQueus.rear = 0;
	memset((u8 *)gatUploadQueus.upload, 0x0, sizeof(gatUploadQueus.upload));
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadQueueisFull(void)
 @Description			���Upload�����Ƿ�����
 @Input				void
 @Return				0  : û��
					1  : ����
**********************************************************************************************************/
u8 GAT_UploadQueueisFull(void)
{
	if ((gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH == gatUploadQueus.front) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadQueueisEmpty(void)
 @Description			���Upload�����Ƿ�Ϊ��
 @Input				void
 @Return				0  : ����������
					1  : ���п�����
**********************************************************************************************************/
u8 GAT_UploadQueueisEmpty(void)
{
	if (gatUploadQueus.front == gatUploadQueus.rear) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_UploadEnqueue(u16 id, u8 direction)
 @Description			��������д�����
 @Input				id 		 : �ش�id��
					direction  : �õشų������� 0 ����� 1 ������
 @Return				void
**********************************************************************************************************/
void GAT_UploadEnqueue(u16 id, u8 direction)
{
	if (GAT_UploadQueueisFull()) {										//��������
		gatUploadQueus.rear = (gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH;		//��βƫ��1
		gatUploadQueus.upload[gatUploadQueus.rear].id = id;					//д���������
		gatUploadQueus.upload[gatUploadQueus.rear].direction = direction;
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//��ͷƫ��1
	}
	else {															//����δ��
		gatUploadQueus.rear = (gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH;		//��βƫ��1
		gatUploadQueus.upload[gatUploadQueus.rear].id = id;					//д���������
		gatUploadQueus.upload[gatUploadQueus.rear].direction = direction;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeue(u16 *id, u8 *direction)
 @Description			�������ݶ�������(����ͷƫ��1)
 @Input				&id		 : ��ŵش�id��ַ
					&direction : ��Ÿõشų��������ַ
 @Return				0 	 	 : ��ȡ�ɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_UploadDequeue(u16 *id, u8 *direction)
{
	if (GAT_UploadQueueisEmpty()) {										//�����ѿ�
		return 1;
	}
	else {															//����δ��
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//��ͷƫ��1
		*id = gatUploadQueus.upload[gatUploadQueus.front].id;					//��ȡ��ͷ����
		*direction = gatUploadQueus.upload[gatUploadQueus.front].direction;
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeueNomove(u16 *id, u8 *direction)
 @Description			�������ݶ�������(����ͷ����)
 @Input				&id		 : ��ŵش�id��ַ
					&direction : ��Ÿõشų��������ַ
 @Return				0 	 	 : ��ȡ�ɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_UploadDequeueNomove(u16 *id, u8 *direction)
{
	u8 frontaddr = 0;
	
	if (GAT_UploadQueueisEmpty()) {										//�����ѿ�
		return 1;
	}
	else {															//����δ��
		frontaddr = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;
		*id = gatUploadQueus.upload[frontaddr].id;							//��ȡ��ͷ����
		*direction = gatUploadQueus.upload[frontaddr].direction;
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeueMove(void)
 @Description			�������ݶ���(����ͷƫ��1)
 @Input				void
 @Return				0 	 	 : ƫ�Ƴɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_UploadDequeueMove(void)
{
	if (GAT_UploadQueueisEmpty()) {										//�����ѿ�
		return 1;
	}
	else {															//����δ��
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//��ͷƫ��1
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_UploadOvertime(void)
 @Description			�����ϴ�����Ӧ��ʱ����(2����û�յ�Ӧ���ٴη���,3�η�����Ӧ��Ͽ�����)(������RTC���ж�)
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_UploadOvertime(void)
{
	if (GATConnect == GAT_ONLINE) {
		if (GATUPLOADAck == GAT_NotReceivedAck) {							//�ȴ�����Ӧ����
			gatOverTime += 1;
			if (gatOverTime >= GATOVERTIME) {								//���ﳬʱʱ��
				gatOverTime = 0;
				GATUPLOADAck = GAT_ReceivedAck;							//�ط�
				gatRepeat += 1;
				if (gatRepeat >= GATREPEAT) {
					gatRepeat = 0;
					GATConnect = GAT_OFFLINE;							//����
				}
			}
		}
	}
	else {
		gatOverTime = 0;
		gatRepeat = 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_InitStatisticalQueue(void)
 @Description			��ʼ��Statistical����
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_InitStatisticalQueue(void)
{
	gatStatisticalQueue.front = 0;
	gatStatisticalQueue.rear = 0;
	memset((u8 *)gatStatisticalQueue.statistical, 0x0, sizeof(gatStatisticalQueue.statistical));
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalQueueisFull(void)
 @Description			���Statistical�����Ƿ�����
 @Input				void
 @Return				0  : û��
					1  : ����
**********************************************************************************************************/
u8 GAT_StatisticalQueueisFull(void)
{
	if ((gatStatisticalQueue.rear + 1) % STATISTICALLENGTH == gatStatisticalQueue.front) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalQueueisEmpty(void)
 @Description			���Statistical�����Ƿ�Ϊ��
 @Input				void
 @Return				0  : ����������
					1  : ���п�����
**********************************************************************************************************/
u8 GAT_StatisticalQueueisEmpty(void)
{
	if (gatStatisticalQueue.front == gatStatisticalQueue.rear) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_StatisticalEnqueue(Gat_StatisticalData *buf)
 @Description			ͳ������д�����
 @Input				Gat_StatisticalData *buf : ������������
 @Return				void
**********************************************************************************************************/
void GAT_StatisticalEnqueue(Gat_StatisticalData *buf)
{
	u8 i = 0;
	
	if (GAT_StatisticalQueueisFull()) {																//��������
		gatStatisticalQueue.rear = (gatStatisticalQueue.rear + 1) % STATISTICALLENGTH;							//��βƫ��1
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].datetime = buf->datetime;						//д���������
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detection_channels = buf->detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].detectionChannelNo = buf->detector_statistical_data[i].detectionChannelNo;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].AcarVolume = buf->detector_statistical_data[i].AcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].BcarVolume = buf->detector_statistical_data[i].BcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].CcarVolume = buf->detector_statistical_data[i].CcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgShare = buf->detector_statistical_data[i].avgShare;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgSpeed = buf->detector_statistical_data[i].avgSpeed;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgLength = buf->detector_statistical_data[i].avgLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgHeadTime = buf->detector_statistical_data[i].avgHeadTime;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].queuingLength = buf->detector_statistical_data[i].queuingLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].reserved = buf->detector_statistical_data[i].reserved;
		}
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//��ͷƫ��1
	}
	else {																						//����δ��
		gatStatisticalQueue.rear = (gatStatisticalQueue.rear + 1) % STATISTICALLENGTH;							//��βƫ��1
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].datetime = buf->datetime;						//д���������
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detection_channels = buf->detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].detectionChannelNo = buf->detector_statistical_data[i].detectionChannelNo;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].AcarVolume = buf->detector_statistical_data[i].AcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].BcarVolume = buf->detector_statistical_data[i].BcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].CcarVolume = buf->detector_statistical_data[i].CcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgShare = buf->detector_statistical_data[i].avgShare;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgSpeed = buf->detector_statistical_data[i].avgSpeed;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgLength = buf->detector_statistical_data[i].avgLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgHeadTime = buf->detector_statistical_data[i].avgHeadTime;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].queuingLength = buf->detector_statistical_data[i].queuingLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].reserved = buf->detector_statistical_data[i].reserved;
		}
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeue(Gat_StatisticalData *buf)
 @Description			ͳ�����ݶ�������(����ͷƫ��1)
 @Input				Gat_StatisticalData *buf : ������������
 @Return				0 	 	 : ��ȡ�ɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_StatisticalDequeue(Gat_StatisticalData *buf)
{
	u8 i = 0;
	
	if (GAT_StatisticalQueueisEmpty()) {																//�����ѿ�
		return 1;
	}
	else {																						//����δ��
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//��ͷƫ��1
		buf->datetime = gatStatisticalQueue.statistical[gatStatisticalQueue.front].datetime;						//��ȡ��ͷ����
		buf->detection_channels = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			buf->detector_statistical_data[i].detectionChannelNo = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].detectionChannelNo;
			buf->detector_statistical_data[i].AcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].AcarVolume;
			buf->detector_statistical_data[i].BcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].BcarVolume;
			buf->detector_statistical_data[i].CcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].CcarVolume;
			buf->detector_statistical_data[i].avgShare = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgShare;
			buf->detector_statistical_data[i].avgSpeed = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgSpeed;
			buf->detector_statistical_data[i].avgLength = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgLength;
			buf->detector_statistical_data[i].avgHeadTime = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgHeadTime;
			buf->detector_statistical_data[i].queuingLength = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].queuingLength;
			buf->detector_statistical_data[i].reserved = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].reserved;
		}
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeueNomove(Gat_StatisticalData *buf)
 @Description			ͳ�����ݶ�������(����ͷ����)
 @Input				Gat_StatisticalData *buf : ������������
 @Return				0 	 	 : ��ȡ�ɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_StatisticalDequeueNomove(Gat_StatisticalData *buf)
{
	u8 i = 0;
	u8 frontaddr = 0;
	
	if (GAT_StatisticalQueueisEmpty()) {																//�����ѿ�
		return 1;
	}
	else {																						//����δ��
		frontaddr = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;
		buf->datetime = gatStatisticalQueue.statistical[frontaddr].datetime;									//��ȡ��ͷ����
		buf->detection_channels = gatStatisticalQueue.statistical[frontaddr].detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			buf->detector_statistical_data[i].detectionChannelNo = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].detectionChannelNo;
			buf->detector_statistical_data[i].AcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].AcarVolume;
			buf->detector_statistical_data[i].BcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].BcarVolume;
			buf->detector_statistical_data[i].CcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].CcarVolume;
			buf->detector_statistical_data[i].avgShare = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgShare;
			buf->detector_statistical_data[i].avgSpeed = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgSpeed;
			buf->detector_statistical_data[i].avgLength = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgLength;
			buf->detector_statistical_data[i].avgHeadTime = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgHeadTime;
			buf->detector_statistical_data[i].queuingLength = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].queuingLength;
			buf->detector_statistical_data[i].reserved = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].reserved;
		}
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeueMove(void)
 @Description			ͳ�����ݶ���(����ͷƫ��1)
 @Input				void
 @Return				0 	 	 : ƫ�Ƴɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 GAT_StatisticalDequeueMove(void)
{
	if (GAT_StatisticalQueueisEmpty()) {																//�����ѿ�
		return 1;
	}
	else {																						//����δ��
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//��ͷƫ��1
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_ImplementEnqueue(u16 statisticaltime)
 @Description			����ͳ��ʱ�佫ͳ�����ݴ������
 @Input				statisticaltime : ͳ��ʱ��
 @Return				void
**********************************************************************************************************/
void GAT_ImplementEnqueue(u16 statisticaltime)
{
	static u16 gatstatisticaltime = 0;																	//gatͳ��ʱ���ʱ��
	u8 channelsnum = 0;																				//gat���ͨ����
	u16 volume = 0;																				//������
	u16 avgoccupancyval = 0;																			//ƽ��ռ��ʱ��
	u16 avgheadtimeval = 0;																			//ƽ����ͷʱ��
	u8 i = 0;
	
	if (statisticaltime != 0) {																		//ͳ��ʱ���費Ϊ0
		gatstatisticaltime += 1;
		if (gatstatisticaltime >= statisticaltime) {														//����ͳ��ʱ��
			gatstatisticaltime = 0;
			
			memset((u8 *)&gatStatisticalData, 0x0, sizeof(gatStatisticalData));								//��ջ�����
			gatStatisticalData.datetime = RTC_GetCounter();												//��ȡͳ���������ɵı���ʱ��
			for (i = 0; i < DetectionChannels; i++) {													//��ȡ����ͨ��id��ͨ����
				if (gatParamEquipment.output_ID[i] != 0x0) {
					channelsnum += 1;
				}
			}
			gatStatisticalData.detection_channels = channelsnum;											//��ȡ���ͨ����
			for (i = 0; i < DetectionChannels; i++) {													//��ȡ����ͨ��ͳ������
				if (gatParamEquipment.output_ID[i] != 0x0) {
					
					volume = calculation_dev.ReadVolume(gatParamEquipment.output_ID[i]);						//��ȡ������ֵ
					if (volume > 255) {
						volume = 255;
					}
					
					avgoccupancyval = calculation_dev.ReadAvgOccupancy(gatParamEquipment.output_ID[i]);			//��ȡƽ��ռ��ʱ��
					avgoccupancyval = avgoccupancyval / statisticaltime / 500;								//ת��Ϊƽ��ռ����
					
					avgheadtimeval = calculation_dev.ReadAvgHeadTime(gatParamEquipment.output_ID[i]);			//��ȡƽ����ͷʱ��
					avgheadtimeval = avgheadtimeval / 1000;												//ת����λΪ��
					if (avgheadtimeval > 255) {
						avgheadtimeval = 255;
					}
					
					gatStatisticalData.detector_statistical_data[i].detectionChannelNo = i + 1;				//���ͨ�����
					gatStatisticalData.detector_statistical_data[i].AcarVolume = 0;							//�����ֳ���C�೵Ϊ�ܳ�����
					gatStatisticalData.detector_statistical_data[i].BcarVolume = 0;
					gatStatisticalData.detector_statistical_data[i].CcarVolume = volume;
					gatStatisticalData.detector_statistical_data[i].avgShare = avgoccupancyval;
					gatStatisticalData.detector_statistical_data[i].avgSpeed = 0;
					gatStatisticalData.detector_statistical_data[i].avgLength = 0;
					gatStatisticalData.detector_statistical_data[i].avgHeadTime = avgheadtimeval;
					gatStatisticalData.detector_statistical_data[i].queuingLength = 0;
				}
			}
			/* �������е���ͳ�����ݴ���ͳ�����ݴ洢���� */
			GAT_StatisticalEnqueue((Gat_StatisticalData *)&gatStatisticalData);
		}
	}
}

/********************************************** END OF FLEE **********************************************/
