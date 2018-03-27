/**
  *********************************************************************************************************
  * @file    gatfunc.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			���� : 
  *			1.  �ӽ��յ�����������ȡ����֡
  *			2.  �����ݱ�ת��Ϊ����֡
  *			3.  ������֡ת��Ϊ���ݱ�
  *			4.  �����ݱ�����ȡ��Ϣ����
  *			5.  �������·��ַ����
  *			6.  �����������·��ַ��ȡ
  *			7.  ���У�����Ƿ���ȷ-----------�������� 1
  *			8.  ���Э��汾�Ƿ����---------�������� 2
  *			9.  �����Ϣ�����Ƿ���ȷ---------�������� 3
  *			10. �����Ϣ���������Ƿ���ȷ-----�������� 4
  *			11. ��ȡ��������ֵ
  *			12. ��ȡ�����ʶֵ
  *********************************************************************************************************
  */

#include "gatfunc.h"
#include "string.h"


/**********************************************************************************************************
 @Function			u8 GAT_LinkAddress_Send(u8 *linkaddress)
 @Description			�������·��ַ����
 @Input				��ż������·���ݵĵ�ַ
 @Return				0 : ERROR
					1 : ���ֽ��ֽ���
					2 : ˫�ֽ��ֽ���
**********************************************************************************************************/
u8 GAT_LinkAddress_Send(u8 *linkaddress)
{
	u8 ubyte = 0;
	u8 i;
	u8 tmpbuf[2] = {0x00, 0x00};
	u16 linknum = LINKADDRESS;
	
	if (linknum > 8191) {												//������Χ
		return 0;
	}
	else if (linknum <= 63) {											//���ֽ�
		tmpbuf[0] |= 0x01;												//��һλ��1
		tmpbuf[0] |= ((linknum << 2) & 0xfc);								//��6λ��ֵ
		ubyte = 1;
	}
	else {															//˫�ֽ�
		tmpbuf[0] |= ((linknum << 2) & 0xfc);								//���ֽڸ�6λ��ֵ
		tmpbuf[1] |= 0x01;												//���ֽڵ�һλ��1
		tmpbuf[1] |= ((linknum >> 5) & 0xfe);
		ubyte = 2;
	}
	
	for (i = 0; i < ubyte; i++) {
		linkaddress[i] = tmpbuf[i];
	}
	
	return ubyte;
}

/**********************************************************************************************************
 @Function			u16 GAT_LinkAddress_Receive(u8 *data_sheet)
 @Description			������������ݱ�����·��ַ��ȡ
 @Input				*data_sheet   				: ���ݱ��ַ
 @Return				��·��ֵַ
**********************************************************************************************************/
u16 GAT_LinkAddress_Receive(u8 *data_sheet)
{
	u16 linknum = 0;
	
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		linknum |= ((data_sheet[0] >> 2) & 0x3f);
	}
	else {															//��·��ַΪ˫�ֽ�
		linknum |= ((data_sheet[0] >> 2) & 0x3f);
		linknum |= ((data_sheet[1] >> 1) & 0x7f) << 6;
	}
	
	return linknum;
}

/**********************************************************************************************************
 @Function			u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length)
 @Description			�ӽ��յ�����������ȡ����֡
 @Input				*receive_buf   			: �������ݻ�����
					data_length 				: �������ݳ���
 @Return				packed_frame_length 		: ����֡����
**********************************************************************************************************/
u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 startflag = 0;
	u16 packed_frame_length = 0, i = 0, j = 0;
	
	for (i = 0; i < receive_length; i++) {
		if ((receive_buf[i] == 0x7E) && (startflag == 0)) {					//֡��ʼ
			tmpbuf[j++] = receive_buf[i];
			startflag = 1;
		}
		else if ((receive_buf[i] == 0x7E) && (startflag == 1)) {				//֡����
			tmpbuf[j++] = receive_buf[i];
			startflag = 0;
			break;
		}
		else if ((receive_buf[i] != 0x7E) && (startflag == 1)) {				//֡������
			tmpbuf[j++] = receive_buf[i];
		}
	}
	
	memset(receive_buf, 0x0, receive_length);
	if ((j != 0) && (startflag == 0)) {									//���յ�����
		for (i = 0; i < j; i++) {
			receive_buf[i] = tmpbuf[i];
		}
		packed_frame_length = j;
	}
	
	return packed_frame_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length)
 @Description			�����ݱ�ת��Ϊ����֡
 @Input				*data_sheet   				: ���ݱ��ַ(�˵�ַָ�����������ݱ�תΪ����֡)
					data_length 				: ���ݱ���
 @Return				packed_frame_length 		: ����֡����
**********************************************************************************************************/
u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 check_num;
	u16 packed_frame_length = 0, i = 0, j =0;
	
	//��ȡ���ݱ�У����
	check_num = data_sheet[0];
	for (i = 1; i < data_length; i++) {
		check_num = check_num ^ data_sheet[i];
	}
	
	//�����ݱ���0x7e,0x7d�ֱ��滻��0x7d,0x5e��0x7d,0x5d
	for (i = 0; i < data_length; i++) {
		if (data_sheet[i] == 0x7e) {
			tmpbuf[j++] = 0x7d;
			tmpbuf[j++] = 0x5e;
		}
		else if (data_sheet[i] == 0x7d) {
			tmpbuf[j++] = 0x7d;
			tmpbuf[j++] = 0x5d;
		}
		else {
			tmpbuf[j++] = data_sheet[i];
		}
	}
	
	//���У����ֵΪ0x7e,0x7d�ֱ��滻��0x7d,0x5e��0x7d,0x5d
	if (check_num == 0x7e) {
		tmpbuf[j++] = 0x7d;
		tmpbuf[j++] = 0x5e;
	}
	else if (check_num == 0x7d) {
		tmpbuf[j++] = 0x7d;
		tmpbuf[j++] = 0x5d;
	}
	else {
		tmpbuf[j++] = check_num;
	}
	
	//����֡��ͷβ0x7e
	data_sheet[0] = 0x7e;
	for (i = 0; i < j; i++) {
		data_sheet[i + 1] = tmpbuf[i];
	}
	data_sheet[j + 1] = 0x7e;
	j = j + 2;
	
	packed_frame_length = j;
	
	return packed_frame_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length)
 @Description			������֡ת��Ϊ���ݱ�
 @Input				*data_frame  	 		: ����֡��ַ(�˵�ַָ������������֡תΪ���ݱ�)
					data_length			: ����֡����
 @Return				sheet_length	    other : ���ݱ���
									   0 : ����
**********************************************************************************************************/
u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u16 sheet_length = data_length;
	u16 i, j = 0;
	
	//�������֡ͷ֡β�Ƿ�Ϊ0x7e
	if ((data_frame[0] != 0x7e) || (data_frame[data_length - 1] != 0x7e)) {
		return 0;
	}
	else {
		sheet_length -= 2;
	}
	
	//���У�����Ƿ�Ϊ1�ֽڻ�2�ֽ�
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			sheet_length -= 2;
		}
		else {
			sheet_length -= 1;
		}
	}
	else {
		sheet_length -= 1;
	}
	
	//������ݱ���0x7d,0x5e��0x7d,0x5d�滻Ϊ0x7e��0x7d
	for (i = 0; i < sheet_length; i++) {
		if (data_frame[i + 1] == 0x7d) {
			if (data_frame[i + 2] == 0x5e) {
				tmpbuf[j++] = 0x7e;
				i++;
			}
			else if (data_frame[i + 2] == 0x5d) {
				tmpbuf[j++] = 0x7d;
				i++;
			}
			else {
				tmpbuf[j++] = data_frame[i + 1];
			}
		}
		else {
			tmpbuf[j++] = data_frame[i + 1];
		}
	}
	
	//������д�뻺��
	for (i = 0; i < j; i++) {
		data_frame[i] = tmpbuf[i];
	}
	
	sheet_length = j;
	
	return sheet_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length)
 @Description			�����ݱ�����ȡ��Ϣ����
 @Input				*data_sheet  	 		: ���ݱ��ַ(�˵�ַָ�����������ݱ�תΪ��Ϣ����)
					data_length			: ���ݱ���
 @Return				message_length			: ��Ϣ���ݳ���
**********************************************************************************************************/
u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u16 message_length = 0;
	u16 message_index = 0;
	u16 i = 0;
	
	message_length = data_length;
	
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		message_index += 1;
		message_length -= 1;
	}
	else {															//��·��ַΪ˫�ֽ�
		message_index += 2;
		message_length -= 2;
	}
	message_index += 3;
	message_length -= 3;
	
	for (i = 0; i < message_length; i++) {
		tmpbuf[i] = data_sheet[message_index + i];
	}
	for (i = 0; i < message_length; i++) {
		data_sheet[i] = tmpbuf[i];
	}
	
	return message_length;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum)
 @Description			�����·��ַ�Ƿ�Ϊ������·��ַ-----�������� 0
 @Input				*data_sheet  	 		: ���ݱ��ַ
					linknum				: �豸��·��ֵַ
 @Return								1	: ����
									0	: ����
**********************************************************************************************************/
u8 GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum)
{
	u8 error = GAT_TRUE;
	u16 RecvLinkAdd = 0;
	
	RecvLinkAdd = GAT_LinkAddress_Receive(data_sheet);
	//���յ�����·��ֵַ���豸��·��ֵַ�Ƚ�
	if (RecvLinkAdd != linknum) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckCode(u8 *data_frame, u16 data_length)
 @Description			���У�����Ƿ���ȷ-----�������� 1
 @Input				*data_frame  	 		: ����֡��ַ
					data_length			: ����֡����
 @Return								1	: ����
									0	: ����
**********************************************************************************************************/
u8 GAT_CheckCode(u8 *data_frame, u16 data_length)
{
	u8 error = GAT_TRUE;
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 checkcode = 0;
	u8 checknum = 0;
	u16 sheet_length = data_length;
	u16 i, j = 0;
	
	//���У�����Ƿ�Ϊ1�ֽڻ�2�ֽ�,����ȡУ����
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			if (data_frame[data_length - 2] == 0x5e) {
				checkcode = 0x7e;
			}
			else if (data_frame[data_length - 2] == 0x5d) {
				checkcode = 0x7d;
			}
		}
		else {
			checkcode = data_frame[data_length - 2];
		}
	}
	else {
		checkcode = data_frame[data_length - 2];
	}
	
	//�������֡ͷ֡β�Ƿ�Ϊ0x7e
	if ((data_frame[0] != 0x7e) || (data_frame[data_length - 1] != 0x7e)) {
		return GAT_FALSE;
	}
	else {
		sheet_length -= 2;
	}
	
	//���У�����Ƿ�Ϊ1�ֽڻ�2�ֽ�
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			sheet_length -= 2;
		}
		else {
			sheet_length -= 1;
		}
	}
	else {
		sheet_length -= 1;
	}
	
	//������ݱ���0x7d,0x5e��0x7d,0x5d�滻Ϊ0x7e��0x7d
	for (i = 0; i < sheet_length; i++) {
		if (data_frame[i + 1] == 0x7d) {
			if (data_frame[i + 2] == 0x5e) {
				tmpbuf[j++] = 0x7e;
				i++;
			}
			else if (data_frame[i + 2] == 0x5d) {
				tmpbuf[j++] = 0x7d;
				i++;
			}
			else {
				tmpbuf[j++] = data_frame[i + 1];
			}
		}
		else {
			tmpbuf[j++] = data_frame[i + 1];
		}
	}
	
	//�������ݱ��У����
	checknum = tmpbuf[0];
	for (i = 1; i < j; i++) {
		checknum = checknum ^ tmpbuf[i];
	}
	
	if (checkcode != checknum) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckVersion(u8 *data_sheet)
 @Description			���Э��汾�Ƿ����-----�������� 2
 @Input				*data_sheet  	 		: ���ݱ��ַ
 @Return								1	: ������
									0	: ����
**********************************************************************************************************/
u8 GAT_CheckVersion(u8 *data_sheet)
{
	u8 error = GAT_TRUE;
	u8 protocolnum = 0;
	
	//��ȡ���ݱ���Э��汾��
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		protocolnum = data_sheet[1];
	}
	else {															//��·��ַΪ˫�ֽ�
		protocolnum = data_sheet[2];
	}
	
	//�Ƚϰ汾��
	if (protocolnum != PROTOCOL_VER) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckVersion(u8 *data_sheet)
 @Description			�����Ϣ�����Ƿ���ȷ-----�������� 3
 @Input				*data_sheet  	 		: ���ݱ��ַ
 @Return								1	: ����
									0	: ����
**********************************************************************************************************/
u8 GAT_CheckMessageType(u8 *data_sheet)
{
	u8 error = GAT_TRUE;
	u8 operationtype = 0;												//��������
	u8 objectid = 0;													//�����ʶ
	
	//��ȡ�������ͺͶ����ʶ
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
	}
	else {															//��·��ַΪ˫�ֽ�
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
	}
	
	//�Ƚϲ��������Ƿ���ȷ
	if ( (operationtype == OPERATIONTYPE_QUERY_REQUEST) || (operationtype == OPERATIONTYPE_SET_REQUEST) ||
		(operationtype == OPERATIONTYPE_ACTIVE_UPLOAD) || (operationtype == OPERATIONTYPE_QUERY_ACK) ||
		(operationtype == OPERATIONTYPE_SET_ACK) || (operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) || 
		(operationtype == OPERATIONTYPE_ERROR_ACK) )
	{
		error = GAT_TRUE;
	}
	else {
		return GAT_FALSE;
	}
	
	//�Ƚ϶����ʶ�Ƿ���ȷ
	if ( (objectid == OBJECTID_ONLINE) || (objectid == OBJECTID_TIME) || (objectid == OBJECTID_BAUD) ||
		(objectid == OBJECTID_CONFIG_PARAM) || (objectid == OBJECTID_STATISTICAL) || (objectid == OBJECTID_HISTORICAL) ||
		(objectid == OBJECTID_PULSE_UPLOADMODE) || (objectid == OBJECTID_PULSE_DATA) || (objectid == OBJECTID_ERROR_MESSAGE) )
	{
		error = GAT_TRUE;
	}
	else {
		return GAT_FALSE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckMessageContent(u8 *data_sheet, u16 data_length)
 @Description			�����Ϣ���������Ƿ���ȷ-----�������� 4
 @Input				*data_sheet  	 		: ���ݱ��ַ
					data_length 			: ���ݱ���
 @Return								1	: ����
									0	: ����
**********************************************************************************************************/
u8 GAT_CheckMessageContent(u8 *data_sheet, u16 data_length)
{
	u8 operationtype = 0;												//��������
	u8 objectid = 0;													//�����ʶ
	u8 detectionchannel = 0;												//���ͨ����
	u16 messagelength = data_length;										//��Ϣ���ݳ���
	
	//��ȡ�������ͺͶ����ʶ
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
		messagelength -= 1;												//ȥ����·��ַ����
	}
	else {															//��·��ַΪ˫�ֽ�
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
		messagelength -= 2;												//ȥ����·��ַ����
	}
	messagelength -= 3;													//ȥ��Э��汾�š��������͡������ʶ 3���ֽڳ���
	
	/* �źŻ����������������ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ��������Ӳ�ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ�����ʱ���������ݱ�, ��Ϣ���� 4 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_TIME)) {
		if (messagelength != 4) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ�����ʱ���ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_TIME)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ����Ͳ������������ݱ�, ��Ϣ���� 4 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_BAUD)) {
		if (messagelength != 4) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ��������ò����������ݱ�, ��Ϣ���� 9 �ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		if (messagelength != 9) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ��������ò�����ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ�����ͳ�����������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_STATISTICAL)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ�������ʷ���ݲ�ѯ���ݱ�, ��Ϣ���� 8 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_HISTORICAL)) {
		if (messagelength != 8) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ��������������ϴ�ģʽ�������ݱ�, ��Ϣ���� ((N + 7) / 8) + 1 �ֽ� ����2�ֽ� */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		if (messagelength < 2) {
			return GAT_FALSE;
		}
		else {
			if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
				detectionchannel = data_sheet[4];
			}
			else {															//��·��ַΪ˫�ֽ�
				detectionchannel = data_sheet[5];
			}
			if ((detectionchannel == 0) || (detectionchannel > 128)) {					//��Χ�ж�
				return GAT_FALSE;
			}
			detectionchannel = (detectionchannel + 7) / 8;							//ת��Ϊ���ͨ���ֽ���
			detectionchannel += 1;												//�Ӽ��ͨ������1�ֽ���
			
			if (messagelength != detectionchannel) {
				return GAT_FALSE;
			}
			else {
				return GAT_TRUE;
			}
		}
	}
	
	/* �źŻ��������������ϴ�ģʽ��ѯ���ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ������������������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_PULSE_DATA)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* �źŻ����ͼ�������������ϴ�Ӧ�����ݱ�, ��Ϣ���� 0 �ֽ� */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_ERROR_MESSAGE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	return GAT_FALSE;
}

/**********************************************************************************************************
 @Function			u8 GAT_GatOperationType(u8 *data_frame)
 @Description			��ȡ��������ֵ
 @Input				*data_sheet  	 		: ���ݱ��ַ
 @Return				operationtype			: ��������(0x80 ~ 0x86)
**********************************************************************************************************/
u8 GAT_GatOperationType(u8 *data_sheet)
{
	u8 operationtype = 0;
	
	//��ȡ��������ֵ
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		operationtype = data_sheet[2];
	}
	else {															//��·��ַΪ˫�ֽ�
		operationtype = data_sheet[3];
	}
	
	return operationtype;
}

/**********************************************************************************************************
 @Function			u8 GAT_GatObjectId(u8 *data_sheet)
 @Description			��ȡ�����ʶֵ
 @Input				*data_sheet  	 		: ���ݱ��ַ
 @Return				objectid				: �����ʶ(0x01 ~ 0x09)
**********************************************************************************************************/
u8 GAT_GatObjectId(u8 *data_sheet)
{
	u8 objectid = 0;
	
	//��ȡ�����ʶֵ
	if ((data_sheet[0] & 0x01) == 1) {										//��·��ַΪ���ֽ�
		objectid = data_sheet[3];
	}
	else {															//��·��ַΪ˫�ֽ�
		objectid = data_sheet[4];
	}
	
	return objectid;
}

/********************************************** END OF FLEE **********************************************/
