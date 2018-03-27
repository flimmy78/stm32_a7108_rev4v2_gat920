#ifndef __GAT_CONNECT_H
#define   __GAT_CONNECT_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


/* Ack Frame Data */
typedef __packed struct
{
	u8  protocolnum;													//Э��汾
	u8  operationtype;													//��������
	u8  objectid;														//�����ʶ
}Gat_AckFrame_Data;

/* ����Ӧ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	u8  errortype;														//��������
}Gat_Error_Data;

/* ʱ���ѯӦ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	u32 datetime;														//����ʱ��ֵ
}Gat_Time_Data;

/* ����������Ӧ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	u8 setbaudresult;													//���������óɹ���־
}Gat_BaudRate_Data;

/* ���������Ӧ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	Gat_Param_Detector ParamDetector;										//���������
}Gat_ConfigParam_Data;

/* ���������ϴ����ò���Ӧ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	Gat_PulseUploadConfig_Data ParamPulseUpload;								//���������ϴ����ò���
}Gat_PulseUploadParam_Data;

/* �������������ϴ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	Gat_PulseUpload_Data ParamPulseMessage;									//���������ź�Դ
}Gat_ActivePulseUpload_Data;

/* ͳ�����������ϴ��� */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//Ӧ���ͷ
	Gat_Statistical_Data ParamStatisticalMessage;							//ͳ������
}Gat_ActiveStatisticalUpload_Data;


u8   GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length);						//�жϴ���ͬ����

void GAT_ErrorDataWrite(u8 errormessage);									//д�����Ӧ����Ϣ�����ͻ�����
u8   GAT_AckSetOnlineWrite(void);											//д����������Ӧ������ͻ��������趨����״̬
u8   GAT_AckQueryOnlineWrite(void);										//�������״̬��д�����Ӳ�ѯӦ������ͻ�����
u8   GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength);							//�������״̬����ȡʱ�����ñ�ʱ�䲢д��ʱ������Ӧ������ͻ�����
u8   GAT_AckQueryTimeWrite(void);											//�������״̬����ȡʱ��д��ʱ���ѯӦ������ͻ�����
u8   GAT_AckSetBaudRateWrite(u8 *msgdata, u16 msglength);						//�������״̬����ȡ���������ñ��������ò����ʲ�д����������Ӧ������ͻ�����
u8   GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength);					//�������״̬����ȡ���ò������ñ����ݲ�д�����ò�������Ӧ������ͻ�����
u8   GAT_AckQueryConfigParamWrite(void);									//�������״̬�������������д������������ѯӦ������ͻ�����
u8   GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength);				//�������״̬����ȡ�����������ò������ñ����ݲ�д�����������ϴ����ò�������Ӧ������ͻ�����
u8   GAT_AckQueryPulseUploadConfigWrite(void);								//�������״̬�������������ϴ����ò���д�����������ϴ����ò�����ѯӦ������ͻ�����

u8   GAT_ActiveUpLoadDataWrite(void);										//�������״̬������������д���������������ϴ������ͻ�����
u8   GAT_AckInspectUpLoadData(void);										//�������״̬�������������������ϴ�Ӧ���������

u8   GAT_ActiveStatisticalDataWrite(void);									//�������״̬����ͳ������д��ͳ�����������ϴ������ͻ�����
u8   GAT_AckInspectStatisticalData(void);									//�������״̬������ͳ�����������ϴ�Ӧ���������

#endif
