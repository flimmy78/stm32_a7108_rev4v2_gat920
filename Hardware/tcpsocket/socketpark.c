/**
  *********************************************************************************************************
  * @file    socketpark.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   Socketͣ�������ݴ���
  *********************************************************************************************************
  * @attention
  *			
  *
  *			
  *********************************************************************************************************
  */

#include "socketpark.h"
#include "socketconfig.h"
#include "socketfunc.h"
#include "calculationconfig.h"
#include "socketinitialization.h"


#ifdef SOCKET_SERIALPORT_USART1
#define SOCKET_USART		USART1
#endif
#ifdef SOCKET_SERIALPORT_USART2
#define SOCKET_USART		USART2
#endif
#ifdef SOCKET_SERIALPORT_USART3
#define SOCKET_USART		USART3
#endif

extern unsigned short 		output_ID[OUTPUT_MAX];

Socket_Packet_Head			SocketParkHeadPacket;							//Socketͣ�������ݰ�ͷ
Socket_Packet_Data			SocketParkDataPacket;							//Socketͣ�������ݰ�


/**********************************************************************************************************
 @Function			void SOCKET_ParkImplement(u8 laneNo, u16 CarNum)
 @Description			Socket����ʵʱ�ϴ�
 @Input				laneNo : ������
					CarNum : ����ֵ
 @Return				void
**********************************************************************************************************/
void SOCKET_ParkImplement(u8 laneNo, u16 CarNum)
{
	u16 headlength = 0;
	u16 datalength = 0;
	
	/* д������ */
	SocketParkHeadPacket.CrossID = socket_dev.GetCrossID();					//д��CrossID
	SocketParkHeadPacket.PacketType = PACKETTYPE_PARKINGLOTDATA;				//ͣ��������
	SocketParkHeadPacket.PacketInfo = 1;									//���ݰ���
	
	SocketParkDataPacket.DeviceType = DEVICETYPE_DICI;						//д��DeviceType
	SocketParkDataPacket.Interval = INTERVALTIME;							//д��Interval
	SocketParkDataPacket.LaneNo = laneNo + 1;								//д��LaneNo
	SocketParkDataPacket.DateTime = RTC_GetCounter();							//д��DateTime
	SocketParkDataPacket.Volume = CarNum;									//д��Volume
	SocketParkDataPacket.Volume1 = 0;										//д��Volume1
	SocketParkDataPacket.Volume2 = 0;										//д��Volume2
	SocketParkDataPacket.Volume3 = 0;										//д��Volume3
	SocketParkDataPacket.Volume4 = 0;										//д��Volume4
	SocketParkDataPacket.Volume5 = 0;										//д��Volume5
	SocketParkDataPacket.AvgOccupancy = 0;									//д��AvgOccupancy
	SocketParkDataPacket.AvgHeadTime = 0;									//д��AvgHeadTime
	SocketParkDataPacket.AvgLength = 0;									//д��AvgLength
	SocketParkDataPacket.AvgSpeed = 0;										//д��AvgSpeed
	SocketParkDataPacket.Saturation = 0;									//д��Saturation
	SocketParkDataPacket.Density = 0;										//д��Density
	SocketParkDataPacket.Pcu = 0;											//д��Pcu
	SocketParkDataPacket.AvgQueueLength = 0;								//д��AvgQueueLength
	
	headlength = SOCKET_ParkObtainPacketHead((u8 *)SocketSendBuf);				//��ȡSocketPark��ͷ���ݲ����뻺��
	datalength = SOCKET_ParkObtainPacketData((u8 *)SocketSendBuf);				//��ȡSocketParkͣ�������ݰ������뻺��
	headlength = headlength + datalength;
	datalength = socket_dev.ObtainPacketManuCheck((u8 *)SocketSendBuf, headlength);
	headlength = headlength + datalength;
	
	SOCKET_USARTSend(SOCKET_USART, (u8 *)SocketSendBuf, headlength);				//��������
}

/**********************************************************************************************************
 @Function			void SOCKET_ParkImplement(u8 laneNo, u16 CarNum)
 @Description			Socket����ʵʱ�ϴ�(�������ϴ�����)
 @Input				*buf				: ���ؽ��յ��شŷ��͵����ݰ�
 @Return				void
**********************************************************************************************************/
void SOCKET_ParkImplementHeartbeat(u8 *buf)
{
	u8 i = 0;
	u16 carnumstate = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//����յ��ش�������
	if ( (phead->type == DATATYPE_HEARTBEAT_WITHOUT_MAGENV) || (phead->type == DATATYPE_HEARTBEAT_WITH_MAGENV) )
	{
		carnumstate = (((~0xffff) | buf[3])<<8) | buf[4];						//��ȡ������״ֵ̬
		for (i = 0; i < OUTPUT_MAX; i++) {									//����ID
			if ((output_ID[i] == (phead->addr_dev)) || (output_ID[i] == 0xFFFF))	//ƥ��ͬID
			{
				SOCKET_ParkImplement(i, carnumstate);
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 SOCKET_ObtainPacketHead(u8 *addr)
 @Description			��ȡSocketPark��ͷ���ݲ����뻺��
 @Input				*addr		: Socket�����ַ
 @Return				���뻺�����ݳ���
					SOCKET_FALSE   : ����
**********************************************************************************************************/
u16 SOCKET_ParkObtainPacketHead(u8 *addr)
{
	u16 headlength = 0;																				//���뻺�����ݳ���
	
	if ((SOCKET_ConvertCrossID((u8 *)&addr[headlength], SocketParkHeadPacket.CrossID)) == SOCKET_FALSE) {			//����CrossID
		return SOCKET_FALSE;
	}
	else {
		headlength += 8;																			//��ַƫ��8byte
	}
	
	addr[headlength] = SocketParkHeadPacket.PacketType;													//������ˮ������
	headlength += 1;																				//��ַƫ��1byte
	
	if ((SOCKET_ConvertPacketInfo((u8 *)&addr[headlength], SocketParkHeadPacket.PacketInfo)) == SOCKET_FALSE) {		//��������������ݰ���
		return SOCKET_FALSE;
	}
	else {
		headlength += 4;																			//��ַƫ��4byte
	}
	
	return headlength;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_ObtainPacketData(u8 *addr)
 @Description			��ȡSocketParkͣ�������ݰ������뻺��
 @Input				*addr		: Socket�����ַ
 @Return				���뻺�����ݳ���
					SOCKET_FALSE   : ����
**********************************************************************************************************/
u16 SOCKET_ParkObtainPacketData(u8 *addr)
{
	u16 datalength = 0;																				//���뻺�����ݳ���
	
	datalength += 13;																				//��ַƫ�Ƴ����ݰ�ͷ
	
	addr[datalength] = SocketParkDataPacket.DeviceType;													//�����豸���
	datalength += 1;																			//��ַƫ��1byte
	
	if ((SOCKET_ConvertInterval((u8 *)&addr[datalength], SocketParkDataPacket.Interval)) == SOCKET_FALSE) {			//����ͳ��ʱ��
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_ConvertLaneNo((u8 *)&addr[datalength], SocketParkDataPacket.LaneNo)) == SOCKET_FALSE) {				//���복����
		return SOCKET_FALSE;
	}
	else {
		datalength += 3;																		//��ַƫ��3byte
	}
	
	if ((SOCKET_ConvertDateTime((u8 *)&addr[datalength], SocketParkDataPacket.DateTime)) == SOCKET_FALSE) {			//������ˮ����ʱ��
		return SOCKET_FALSE;
	}
	else {
		datalength += 20;																		//��ַƫ��20byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume)) == SOCKET_FALSE) {				//����һ��ͨ������
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume1)) == SOCKET_FALSE) {				//����΢С��������
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume2)) == SOCKET_FALSE) {				//����С������
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume3)) == SOCKET_FALSE) {				//�����г�����
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume4)) == SOCKET_FALSE) {				//���������
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Volume5)) == SOCKET_FALSE) {				//���볬������
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.AvgOccupancy)) == SOCKET_FALSE) {			//����ƽ��ռ��ʱ��
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.AvgHeadTime)) == SOCKET_FALSE) {			//����ƽ����ͷʱ��
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketParkDataPacket.AvgLength)) == SOCKET_FALSE) {			//����ƽ������
		return SOCKET_FALSE;
	}
	else {
		datalength += 4;																		//��ַƫ��4byte
	}
	
	if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketParkDataPacket.AvgSpeed)) == SOCKET_FALSE) {			//����ƽ���ٶ�
		return SOCKET_FALSE;
	}
	else {
		datalength += 4;																		//��ַƫ��4byte
	}
	
	addr[datalength] = SocketParkDataPacket.Saturation;													//���뱥�Ͷ�
	datalength += 1;																			//��ַƫ��1byte
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Density)) == SOCKET_FALSE) {				//�����ܶ�
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketParkDataPacket.Pcu)) == SOCKET_FALSE) {				//���뵱��С����
		return SOCKET_FALSE;
	}
	else {
		datalength += 2;																		//��ַƫ��2byte
	}
	
	if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketParkDataPacket.AvgQueueLength)) == SOCKET_FALSE) {		//�����Ŷӳ���
		return SOCKET_FALSE;
	}
	else {
		datalength += 4;																		//��ַƫ��4byte
	}
	
	datalength -= 13;																			//��ȥ���ݰ�ͷ13byte
	
	return datalength;
}

/********************************************** END OF FLEE **********************************************/
