/**
  *********************************************************************************************************
  * @file    gatconnect.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  �жϴ���ͬ����
  *
  *			2.  д�����Ӧ����Ϣ�����ͻ�����
  *			3.  д����������Ӧ������ͻ��������趨����״̬
  *			4.  �������״̬��д�����Ӳ�ѯӦ������ͻ�����
  *			5.  �������״̬����ȡʱ�����ñ�ʱ�䲢д��ʱ������Ӧ������ͻ�����
  *			6.  �������״̬����ȡʱ��д��ʱ���ѯӦ������ͻ�����
  *			7.  �������״̬����ȡ���������ñ��������ò����ʲ�д����������Ӧ������ͻ�����
  *			8.  �������״̬����ȡ���ò������ñ����ݲ�д�����ò�������Ӧ������ͻ�����
  *			9.  �������״̬�������������д������������ѯӦ������ͻ�����
  *			10. �������״̬����ȡ�����������ò������ñ����ݲ�д�����������ϴ����ò�������Ӧ������ͻ�����
  *			11. �������״̬�������������ϴ����ò���д�����������ϴ����ò�����ѯӦ������ͻ�����
  *
  *			12. �������״̬������������д���������������ϴ������ͻ�����
  *			13. �������״̬�������������������ϴ�Ӧ���������
  *
  *			14. �������״̬����ͳ������д��ͳ�����������ϴ������ͻ�����
  *			15. �������״̬������ͳ�����������ϴ�Ӧ���������
  *********************************************************************************************************
  */

#include "gatconnect.h"
#include "gatfunc.h"
#include "gatserial.h"
#include "gatupload.h"
#include "string.h"


/**********************************************************************************************************
 @Function			u8 GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length)
 @Description			�жϴ���ͬ����
 @Input				*data_sheet  	 		: ���ݱ��ַ
					data_length 			: ���ݱ���
 @Return				error			   0 : ����
									   1 : ����
**********************************************************************************************************/
u8 GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length)
{
	u8 error = GAT_TRUE;
	u8 operationtype = 0;												//��������
	u8 objectid = 0;													//�����ʶ
	u8 *message_data;													//��Ϣ����
	u16 message_length = 0;												//��Ϣ����
	
	//��ȡ�������ͺͶ����ʶ
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
	}
	else {															//��·��ַΪ˫�ֽ�
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
	}
	
	//��ȡ��Ϣ���ݺ���Ϣ����
	message_data = data_sheet;
	message_length = GAT_DataSheetToMessage(message_data, data_length);
	
	/* �źŻ����������������ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		error = GAT_AckSetOnlineWrite();									//���������������
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ��������Ӳ�ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		error = GAT_AckQueryOnlineWrite();									//�������Ӳ�ѯ����
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ�����ʱ���������ݱ�, ��Ϣ���� 4 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_TIME)) {
		error = GAT_AckSetTimeWrite(message_data, message_length);				//����ʱ�����ñ���
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ�����ʱ���ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_TIME)) {
		error = GAT_AckQueryTimeWrite();									//����ʱ���ѯ����
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ����Ͳ������������ݱ�, ��Ϣ���� 4 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_BAUD)) {
		error = GAT_AckSetBaudRateWrite(message_data, message_length);			//���ղ��������ñ���
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ��������ò����������ݱ�, ��Ϣ���� 9 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		error = GAT_AckSetConfigParamWrite(message_data, message_length);		//�������ò������ñ���
		if (error == GAT_TRUE) {
			return GAT_TRUE;
		}
		else if (error == GAT_DATAFALSE) {
			return GAT_DATAFALSE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ��������ò�����ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		error = GAT_AckQueryConfigParamWrite();								//�������ò�����ѯ����
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ�����ͳ�����������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_STATISTICAL)) {
		error = GAT_AckInspectStatisticalData();							//����ͳ�������ϴ�Ӧ�����ݱ���
		if (error == GAT_RECEIVEEVENT) {
			return GAT_RECEIVEEVENT;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ�������ʷ���ݲ�ѯ���ݱ�, ��Ϣ���� 8 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_HISTORICAL)) {
		
	}
	
	/* �źŻ��������������ϴ�ģʽ�������ݱ�, ��Ϣ���� ((N + 7) / 8) + 1 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		error = GAT_AckSetPulseUploadConfigWrite(message_data, message_length);	//�������������ϴ�ģʽ���ñ���
		if (error == GAT_TRUE) {
			return GAT_TRUE;
		}
		else if (error == GAT_DATAFALSE) {
			return GAT_DATAFALSE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ��������������ϴ�ģʽ��ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		error = GAT_AckQueryPulseUploadConfigWrite();						//�������������ϴ�ģʽ��ѯ����
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ������������������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_PULSE_DATA)) {
		error = GAT_AckInspectUpLoadData();								//�������������ϴ�Ӧ�����ݱ���
		if (error == GAT_RECEIVEEVENT) {
			return GAT_RECEIVEEVENT;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* �źŻ����ͼ�������������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_ERROR_MESSAGE)) {
		
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			void GAT_ErrorDataWrite(u8 errormessage)
 @Description			д�����Ӧ����Ϣ�����ͻ�����
 @Input				errormessage	: ������Ϣ
 @Return				void
**********************************************************************************************************/
void GAT_ErrorDataWrite(u8 errormessage)
{
	u8 linkaddresslength = 0;																	//��·��ַ����
	Gat_Error_Data tmpbuf;
	
	tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//Э��汾
	tmpbuf.AckHead.operationtype = OPERATIONTYPE_ERROR_ACK;											//��������->����Ӧ��
	tmpbuf.AckHead.objectid = OBJECTID_ERROR_MESSAGE;													//�����ʶ->������Ϣ
	tmpbuf.errortype = errormessage;																//��������
	
	linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
	memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д�����Ӧ���
	GATSendLength = linkaddresslength + sizeof(tmpbuf);												//д�뷢�����ݳ���
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetOnlineWrite(void)
 @Description			д����������Ӧ������ͻ��������趨����״̬
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckSetOnlineWrite(void)
{
	u8 linkaddresslength = 0;																	//��·��ַ����
	Gat_AckFrame_Data tmpbuf;
	
	tmpbuf.protocolnum = PROTOCOL_VER;																//Э��汾
	tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;													//��������->����Ӧ��
	tmpbuf.objectid = OBJECTID_ONLINE;																//�����ʶ->�������
	
	linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
	memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д����������Ӧ���
	GATSendLength = linkaddresslength + sizeof(tmpbuf);												//д�뷢�����ݳ���
	
	GATConnect = GAT_ONLINE;																		//����״̬��Ϊ������
	
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryOnlineWrite(void)
 @Description			�������״̬��д�����Ӳ�ѯӦ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckQueryOnlineWrite(void)
{
	u8 linkaddresslength = 0;																	//��·��ַ����
	Gat_AckFrame_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																//������
		tmpbuf.protocolnum = PROTOCOL_VER;															//Э��汾
		tmpbuf.operationtype = OPERATIONTYPE_QUERY_ACK;												//��������->��ѯӦ��
		tmpbuf.objectid = OBJECTID_ONLINE;															//�����ʶ->�������
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//д�����Ӳ�ѯӦ���
		GATSendLength = linkaddresslength + sizeof(tmpbuf);											//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																					//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
 @Description			�������״̬����ȡʱ�����ñ�ʱ�䲢д��ʱ������Ӧ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				msgdata	: ��Ϣ����
					msglength	: ��Ϣ����
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																	//��·��ַ����
	Gat_AckFrame_Data tmpbuf;
	u32 datetime = 0;
	
	if (GATConnect == GAT_ONLINE) {																//������
		if (msglength == 4) {																	//�ֽ������
			datetime |= msgdata[0];																//��ȡʱ��ֵ
			datetime |= msgdata[1] << 8;
			datetime |= msgdata[2] << 16;
			datetime |= msgdata[3] << 24;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
			PWR_BackupAccessCmd(ENABLE);
			RTC_WaitForLastTask();
			RTC_SetCounter(datetime);															//д��RTC�Ĵ���ֵ
			RTC_WaitForLastTask();
			
			tmpbuf.protocolnum = PROTOCOL_VER;														//Э��汾
			tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;											//��������->����Ӧ��
			tmpbuf.objectid = OBJECTID_TIME;														//�����ʶ->��ʱ
			
			linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);									//д����·��ַ
			memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));				//д��ʱ������Ӧ���
			GATSendLength = linkaddresslength + sizeof(tmpbuf);										//д�뷢�����ݳ���
			
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																					//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryTimeWrite(void)
 @Description			�������״̬����ȡʱ��д��ʱ���ѯӦ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckQueryTimeWrite(void)
{
	u8 linkaddresslength = 0;																	//��·��ַ����
	Gat_Time_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																//������
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;													//Э��汾
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;										//��������->��ѯӦ��
		tmpbuf.AckHead.objectid = OBJECTID_TIME;													//�����ʶ->��ʱ
		tmpbuf.datetime = RTC_GetCounter();														//��ȡ����ʱ��ֵ
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//д��ʱ���ѯӦ���
		GATSendLength = linkaddresslength + sizeof(tmpbuf);											//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																					//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
 @Description			�������״̬����ȡ���������ñ��������ò����ʲ�д�벨��������Ӧ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				msgdata	: ��Ϣ����
					msglength	: ��Ϣ����
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckSetBaudRateWrite(u8 *msgdata, u16 msglength)
{
	u8 error = GAT_TRUE;
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_BaudRate_Data tmpbuf;
	u32 baudrate = 0;
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {															//����SNѡ��Gat920���Ӵ���1
		GAT920_USART = USART1;
	}
	else {																						//����SNѡ��Gat920���Ӵ���2
		GAT920_USART = USART2;
	}
	
	if (GATConnect == GAT_ONLINE) {																	//������
		if (msglength == 4) {																		//�ֽ������
			baudrate |= msgdata[0];																	//��ȡ������ֵ
			baudrate |= msgdata[1] << 8;
			baudrate |= msgdata[2] << 16;
			baudrate |= msgdata[3] << 24;
			gatParamEquipment.baudRate = baudrate;														//����������ֵ
			error = GAT_PortSerialInit(GAT920_USART, gatParamEquipment.baudRate, 8, GAT_PAR_NONE);
			if (error != GAT_FALSE) {
				tmpbuf.setbaudresult = 1;															//���óɹ�
			}
			else {
				tmpbuf.setbaudresult = 0;															//����ʧ��
			}
			
			tmpbuf.AckHead.protocolnum = PROTOCOL_VER;													//Э��汾
			tmpbuf.AckHead.operationtype = OPERATIONTYPE_SET_ACK;											//��������->����Ӧ��
			tmpbuf.AckHead.objectid = OBJECTID_BAUD;													//�����ʶ->ͨ�Ų�����
			
			linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//д����·��ַ
			memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//д�벨��������Ӧ���
			GATSendLength = linkaddresslength + sizeof(tmpbuf);											//д�뷢�����ݳ���
			
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength)
 @Description			�������״̬����ȡ���ò������ñ����ݲ�д�����ò�������Ӧ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				msgdata		: ��Ϣ����
					msglength		: ��Ϣ����
 @Return				GAT_TRUE  	: ��ȷ
					GAT_FALSE 	: ����
					GAT_DATAFALSE	: ��Ϣ���ݴ���
**********************************************************************************************************/
u8 GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_AckFrame_Data tmpbuf;
	Gat_Param_Detector_Config configparam;																//���ò���
	
	memset((u8 *)&configparam, 0x0, sizeof(configparam));													//��ջ���ջ
	
	if (GATConnect == GAT_ONLINE) {																	//������
		if (msglength == 9) {																		//�ֽ������
			configparam.statistical_period |= msgdata[0];												//��ȡ����ֵ
			configparam.statistical_period |= msgdata[1] << 8;
			configparam.Acar_langth |= msgdata[2];
			configparam.Bcar_length |= msgdata[3];
			configparam.Ccar_length |= msgdata[4];
			configparam.reserved |= msgdata[5];
			configparam.reserved |= msgdata[6];
			configparam.reserved |= msgdata[7];
			configparam.reserved |= msgdata[8];
			
			/* �����Ϣ���������Ƿ���ȷ */
			if ( (configparam.statistical_period > 1000) || (configparam.Acar_langth > 255) ||
				(configparam.Bcar_length > 150) || (configparam.Ccar_length > 50) ) {							//��Ϣ���ݴ���
				return GAT_DATAFALSE;
			}
			else {																				//��Ϣ������ȷ
				gatParamDetector.detector_config.statistical_period = configparam.statistical_period;
				gatParamDetector.detector_config.Acar_langth = configparam.Acar_langth;
				gatParamDetector.detector_config.Bcar_length = configparam.Bcar_length;
				gatParamDetector.detector_config.Ccar_length = configparam.Ccar_length;
				gatParamDetector.detector_config.reserved = configparam.reserved;
				GAT_ParamSaveToFlash();																//���ò�������Flash
				tmpbuf.protocolnum = PROTOCOL_VER;														//Э��汾
				tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;											//��������->����Ӧ��
				tmpbuf.objectid = OBJECTID_CONFIG_PARAM;												//�����ʶ->���ò���
				
				linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);									//д����·��ַ
				memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));				//д�����ò�������Ӧ���
				GATSendLength = linkaddresslength + sizeof(tmpbuf);										//д�뷢�����ݳ���
				
				return GAT_TRUE;
			}
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryConfigParamWrite(void)
 @Description			�������״̬�������������д������������ѯӦ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckQueryConfigParamWrite(void)
{
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_ConfigParam_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																	//������
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//Э��汾
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;											//��������->��ѯӦ��
		tmpbuf.AckHead.objectid = OBJECTID_CONFIG_PARAM;													//�����ʶ->���ò���
		
		tmpbuf.ParamDetector.manufacturers_length = gatParamDetector.manufacturers_length;						//����������������ֽ���
		strcpy((char *)tmpbuf.ParamDetector.manufacturers_name, (char *)gatParamDetector.manufacturers_name);		//���������������
		tmpbuf.ParamDetector.model_length = gatParamDetector.model_length;									//������ͺŵ��ֽ���
		strcpy((char *)tmpbuf.ParamDetector.model_name, (char *)gatParamDetector.model_name);					//��������ͺ�
		tmpbuf.ParamDetector.detection_channels = gatParamDetector.detection_channels;							//�����ͨ����
		tmpbuf.ParamDetector.statistical_mode = gatParamDetector.statistical_mode;								//ͳ�Ʒ�ʽ
		tmpbuf.ParamDetector.avgshare = gatParamDetector.avgshare;											//����ƽ��ռ����
		tmpbuf.ParamDetector.avgspeed = gatParamDetector.avgspeed;											//����ƽ����ʻ�ٶ�
		tmpbuf.ParamDetector.avglength = gatParamDetector.avglength;										//����ƽ������
		tmpbuf.ParamDetector.avgheadtime = gatParamDetector.avgheadtime;										//����ƽ����ͷʱ��
		tmpbuf.ParamDetector.queuing_length = gatParamDetector.queuing_length;								//�����Ŷӳ���
		tmpbuf.ParamDetector.reserved = gatParamDetector.reserved;											//�����ֶ�
		tmpbuf.ParamDetector.detection_mode = gatParamDetector.detection_mode;								//����ֶ�
		tmpbuf.ParamDetector.signal_output_delay = gatParamDetector.signal_output_delay;						//�ź������ʱ
		tmpbuf.ParamDetector.detector_config.statistical_period = gatParamDetector.detector_config.statistical_period;//ͳ�����ݵļ�������
		tmpbuf.ParamDetector.detector_config.Acar_langth = gatParamDetector.detector_config.Acar_langth;			//A�೵����
		tmpbuf.ParamDetector.detector_config.Bcar_length = gatParamDetector.detector_config.Bcar_length;			//B�೵����
		tmpbuf.ParamDetector.detector_config.Ccar_length = gatParamDetector.detector_config.Ccar_length;			//C�೵����
		tmpbuf.ParamDetector.detector_config.reserved = gatParamDetector.detector_config.reserved;
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д�����ò�����ѯӦ���
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength)
 @Description			�������״̬����ȡ�����������ò������ñ����ݲ�д�����������ϴ����ò�������Ӧ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				msgdata		: ��Ϣ����
					msglength		: ��Ϣ����
 @Return				GAT_TRUE  	: ��ȷ
					GAT_FALSE 	: ����
					GAT_DATAFALSE	: ��Ϣ���ݴ���
**********************************************************************************************************/
u8 GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																		//��·��ַ����
	u16 channelbyte = 0, i = 0;
	Gat_AckFrame_Data tmpbuf;
	Gat_PulseUploadConfig_Data configparam;																//���������ϴ����ò���
	
	if (msglength < 2) {
		return GAT_DATAFALSE;
	}
	memset((u8 *)&configparam, 0x0, sizeof(configparam));													//��ջ���ջ
	channelbyte = msglength - 1;
	
	if (GATConnect == GAT_ONLINE) {																	//������
		configparam.pulseUploadChannel |= msgdata[0];													//��ȡ����ֵ
		for (i = 0; i < channelbyte; i++) {
			configparam.pulseUploadBit[i] |= msgdata[i + 1];
		}
		
		gatParamPulse.pulseUploadChannel = configparam.pulseUploadChannel;									//�����ϴ����ͨ����
		for (i = 0; i < 16; i++) {
			gatParamPulse.pulseUploadBit[i] = configparam.pulseUploadBit[i];									//���������ϴ�ͨ��ʹ��λ
		}
		GAT_ParamSaveToFlash();																		//���ò�������Flash
		
		tmpbuf.protocolnum = PROTOCOL_VER;																//Э��汾
		tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;													//��������->����Ӧ��
		tmpbuf.objectid = OBJECTID_PULSE_UPLOADMODE;														//�����ʶ->���������ϴ�ģʽ
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д�����ò�������Ӧ���
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryPulseUploadConfigWrite(void)
 @Description			�������״̬�������������ϴ����ò���д�����������ϴ����ò�����ѯӦ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_AckQueryPulseUploadConfigWrite(void)
{
	u8 i = 0;
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_PulseUploadParam_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//��ջ���ջ
	
	if (GATConnect == GAT_ONLINE) {																	//������
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//Э��汾
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;											//��������->��ѯӦ��
		tmpbuf.AckHead.objectid = OBJECTID_PULSE_UPLOADMODE;												//�����ʶ->���������ϴ�ģʽ
		
		tmpbuf.ParamPulseUpload.pulseUploadChannel = gatParamPulse.pulseUploadChannel;							//�����ϴ����ͨ����
		for (i = 0; i < 16; i++) {
			tmpbuf.ParamPulseUpload.pulseUploadBit[i] = gatParamPulse.pulseUploadBit[i];						//���������ϴ�ͨ��ʹ��λ
		}
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д�����ò�����ѯӦ���
		
		i = (gatParamPulse.pulseUploadChannel + 7) / 8;													//ת��Ϊ���ͨ���ֽ���
		i += 1;																					//�Ӽ��ͨ������1�ֽ���
		
		GATSendLength = linkaddresslength + 3 + i;														//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_ActiveUpLoadDataWrite(void)
 @Description			�������״̬������������д���������������ϴ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_ActiveUpLoadDataWrite(void)
{
	u16 id = 0;
	u8 direction = 0;
	u8 channelnum = 0;
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_ActivePulseUpload_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//��ջ���ջ
	
	if (GATConnect == GAT_ONLINE) {																	//������
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//Э��汾
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_ACTIVE_UPLOAD;											//��������->�����ϴ�
		tmpbuf.AckHead.objectid = OBJECTID_PULSE_DATA;													//�����ʶ->��������
		
		if (GAT_UploadDequeueNomove(&id, &direction) == 1) {												//���������ݲ���ȡ��ͷ����
			return GAT_FALSE;
		}
		for (channelnum = 0; channelnum < DetectionChannels; channelnum++) {
			if (gatParamEquipment.output_ID[channelnum] == id) {
				break;
			}
		}
		channelnum = channelnum + 1;
		
		tmpbuf.ParamPulseMessage.pulseUploadChannelNo = channelnum;											//���ͨ�����
		tmpbuf.ParamPulseMessage.pulseUploadCarInOut = direction;											//��������
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д���������������ϴ���
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//д�뷢�����ݳ���
		
		return GAT_TRUE;
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckInspectUpLoadData(void)
 @Description			�������״̬�������������������ϴ�Ӧ���������
 @Input				void
 @Return				GAT_RECEIVEEVENT    : ��ȷ
					GAT_FALSE 		: ����
**********************************************************************************************************/
u8 GAT_AckInspectUpLoadData(void)
{
	u8 error = GAT_TRUE;
	
	if (GATConnect == GAT_ONLINE) {																	//������
		if (GATUPLOADAck == GAT_ReceivedAck) {															//��δ�����ϴ����յ�Ӧ��
			return GAT_FALSE;
		}
		else {
			error = GAT_UploadDequeueMove();															//�������ݶ���(����ͷƫ��1)
			if (error != GAT_TRUE) {
				return GAT_FALSE;
			}
			GATUPLOADAck = GAT_ReceivedAck;															//�յ������ϴ�Ӧ��
			gatOverTime = 0;
			gatRepeat = 0;
			
			return GAT_RECEIVEEVENT;
		}
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_ActiveStatisticalDataWrite(void)
 @Description			�������״̬����ͳ������д��ͳ�����������ϴ������ͻ�����(����д��, δ���Ӳ�д��)
 @Input				void
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_ActiveStatisticalDataWrite(void)
{
	u8 i = 0;
	u8 linkaddresslength = 0;																		//��·��ַ����
	Gat_ActiveStatisticalUpload_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//��ջ���ջ
	
	if (GATConnect == GAT_ONLINE) {																	//������
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//Э��汾
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_ACTIVE_UPLOAD;											//��������->�����ϴ�
		tmpbuf.AckHead.objectid = OBJECTID_STATISTICAL;													//�����ʶ->ͳ������
		/* ��ȡͳ�����ݴ洢�������� */
		memset((u8 *)&gatStatisticalData, 0x0, sizeof(gatStatisticalData));									//��ջ�����
		if (GAT_StatisticalDequeueNomove((Gat_StatisticalData *)&gatStatisticalData) == 1) {						//���������ݲ���ȡ��ͷ����
			return GAT_FALSE;
		}
		/* д��ͳ�����������ϴ��� */
		tmpbuf.ParamStatisticalMessage.DateTime = gatStatisticalData.datetime;											//ͳ���������ɵı���ʱ��
		tmpbuf.ParamStatisticalMessage.detector_config.statistical_period = gatParamDetector.detector_config.statistical_period;	//ͳ�����ݵļ�������
		tmpbuf.ParamStatisticalMessage.detector_config.Acar_langth = gatParamDetector.detector_config.Acar_langth;				//A�೵����
		tmpbuf.ParamStatisticalMessage.detector_config.Bcar_length = gatParamDetector.detector_config.Bcar_length;				//B�೵����
		tmpbuf.ParamStatisticalMessage.detector_config.Ccar_length = gatParamDetector.detector_config.Ccar_length;				//C�೵����
		tmpbuf.ParamStatisticalMessage.detector_config.reserved = gatParamDetector.detector_config.reserved;					//�����ֶ�
		tmpbuf.ParamStatisticalMessage.detection_channels = gatStatisticalData.detection_channels;							//��ȡ���ͨ����
		
		for (i = 0; i < gatStatisticalData.detection_channels; i++) {
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].detectionChannelNo = gatStatisticalData.detector_statistical_data[i].detectionChannelNo;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].AcarVolume = gatStatisticalData.detector_statistical_data[i].AcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].BcarVolume = gatStatisticalData.detector_statistical_data[i].BcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].CcarVolume = gatStatisticalData.detector_statistical_data[i].CcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgShare = gatStatisticalData.detector_statistical_data[i].avgShare;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgSpeed = gatStatisticalData.detector_statistical_data[i].avgSpeed;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgLength = gatStatisticalData.detector_statistical_data[i].avgLength;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgHeadTime = gatStatisticalData.detector_statistical_data[i].avgHeadTime;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].queuingLength = gatStatisticalData.detector_statistical_data[i].queuingLength;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].reserved = gatStatisticalData.detector_statistical_data[i].reserved;
		}
		
		/* ͳ�����������ϴ���д�뷢�ͻ����� */
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//д����·��ַ
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//д��ͳ�����������ϴ���
		/* д�뷢�����ݳ��� */
		GATSendLength = linkaddresslength + sizeof(tmpbuf) - ((DetectionChannels - tmpbuf.ParamStatisticalMessage.detection_channels) * sizeof(Gat_DetectionChannel_Data));
		
		return GAT_TRUE;
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckInspectStatisticalData(void)
 @Description			�������״̬������ͳ�����������ϴ�Ӧ���������
 @Input				void
 @Return				GAT_RECEIVEEVENT    : ��ȷ
					GAT_FALSE 		: ����
**********************************************************************************************************/
u8 GAT_AckInspectStatisticalData(void)
{
	u8 error = GAT_TRUE;
	
	if (GATConnect == GAT_ONLINE) {																	//������
		if (GATUPLOADAck == GAT_ReceivedAck) {															//��δ�����ϴ����յ�Ӧ��
			return GAT_FALSE;
		}
		else {
			error = GAT_StatisticalDequeueMove();														//�������ݶ���(����ͷƫ��1)
			if (error != GAT_TRUE) {
				return GAT_FALSE;
			}
			GATUPLOADAck = GAT_ReceivedAck;															//�յ������ϴ�Ӧ��
			gatOverTime = 0;
			gatRepeat = 0;
			
			return GAT_RECEIVEEVENT;
		}
	}
	else {																						//δ����
		return GAT_FALSE;
	}
}

/********************************************** END OF FLEE **********************************************/
