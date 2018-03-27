#ifndef __SOCKET_FUNC_H
#define   __SOCKET_FUNC_H

#include "stm32f10x_lib.h"
#include "socketconfig.h"

extern Socket_Packet_Head			SocketHeadPacket;						//Socket���ݰ�ͷ
extern Socket_Packet_Data			SocketDataPacket[OUTPUT_MAX];				//Socket�������ݰ���
extern volatile u8 SocketSendBuf[SOCKET_CACHE_SIZE];							//Socket�������ݻ���
extern volatile u8 SocketReceiveBuf[SOCKET_RECVIVE_SIZE];						//Socket�������ݻ���


u16  SOCKET_ObtainPacketHead(u8 *addr);										//��ȡSocket��ͷ���ݲ����뻺��
u16  SOCKET_ObtainPacketData(u8 *addr);										//��ȡSocket�������ݰ������뻺��
u16  SOCKET_ObtainPacketManuCheck(u8 *addr, u16 buflength);						//����У���벢���������̱����У����


u8   SOCKET_ConvertCrossID(u8 *addr, u32 crossid);							//��crossidת��Ϊ�ַ���ʽ�����뻺��
u8   SOCKET_ConvertPacketInfo(u8 *addr, u32 packetinfo);						//��packetinfoת�������뻺��
u8   SOCKET_ConvertInterval(u8 *addr, u16 interval);							//��intervalת�������뻺��
u8   SOCKET_ConvertLaneNo(u8 *addr, u16 lanno);								//��lannoת��Ϊ�ַ���ʽ�����뻺��
u8   SOCKET_ConvertDateTime(u8 *addr, u32 rtccounter);							//��ϵͳʱ��ת��Ϊ�ַ���ʽ�����뻺��
u8   SOCKET_Convert2Byte(u8 *addr, u16 data);								//��2Byte����ת�������뻺��
u8   SOCKET_Convert4Byte(u8 *addr, float data);								//��float����ת�������뻺��
u8	SOCKET_Convert4ByteInt(u8 *addr, u32 data);								//��Int����ת�������뻺��

void SOCKET_GetOutputID(u16 *outputid);										//��ȡoutput_ID����˿ڵĲ�����Socket�������ݰ�
u16  SOCKET_PackagedStorage(void);											//Socket�����ݴ�����뻺����

#endif
