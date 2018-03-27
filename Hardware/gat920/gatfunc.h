#ifndef __GAT_FUNC_H
#define   __GAT_FUNC_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length);				//�ӽ��յ�����������ȡ����֡
u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length);					//�����ݱ�ת��Ϊ����֡
u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length);					//������֡ת��Ϊ���ݱ�
u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length);					//�����ݱ�����ȡ��Ϣ����

u8  GAT_LinkAddress_Send(u8 *linkaddress);									//�������·��ַ����
u16 GAT_LinkAddress_Receive(u8 *data_sheet);									//�����������·��ַ��ȡ

u8  GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum);							//�����·��ַ�Ƿ�Ϊ������·��ַ-----�������� 0
u8  GAT_CheckCode(u8 *data_frame, u16 data_length);							//���У�����Ƿ���ȷ-----------------�������� 1
u8  GAT_CheckVersion(u8 *data_sheet);										//���Э��汾�Ƿ����---------------�������� 2
u8  GAT_CheckMessageType(u8 *data_sheet);									//�����Ϣ�����Ƿ���ȷ---------------�������� 3
u8  GAT_CheckMessageContent(u8 *data_sheet, u16 data_length);					//�����Ϣ���������Ƿ���ȷ-----------�������� 4

u8  GAT_GatOperationType(u8 *data_sheet);									//��ȡ��������ֵ
u8  GAT_GatObjectId(u8 *data_sheet);										//��ȡ�����ʶֵ

#endif
