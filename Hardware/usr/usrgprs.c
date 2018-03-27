/**
  *********************************************************************************************************
  * @file    usrgprs.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR-GPRS-7S3��������
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrgprs.h"
#include "usrserial.h"
#include "string.h"
#include "stdio.h"
#include "socketconfig.h"


extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];
extern void delay_1ms(u32 nCount);
extern mvb_param_net_config		param_net_cfg;									//�������ò���

u8 usrtmpbuf[USR_CACHE_SIZE];
u8 tmpbuf1[20];
u8 tmpbuf2[10];

/**********************************************************************************************************
 @Function			void USRGPRS_ReadModbusPrarm(void)
 @Description			��ȡ����Modbus����
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRGPRS_ReadModbusPrarm(void)
{
	USRCheckChange.socketA_addr_h = param_net_cfg.socketA_addr_h;
	USRCheckChange.socketA_addr_l = param_net_cfg.socketA_addr_l;
	USRCheckChange.socketA_port = param_net_cfg.socketA_port;
	USRCheckChange.socketA_connect_mode = param_net_cfg.socketA_connect_mode;
}

/**********************************************************************************************************
 @Function			void USRGPRS_MonitorChange(void)
 @Description			����GPRS�����Ƿ�ı�
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRGPRS_MonitorChange(void)
{
	if ( (USRCheckChange.socketA_addr_h != param_net_cfg.socketA_addr_h) || 
		(USRCheckChange.socketA_addr_l != param_net_cfg.socketA_addr_l) || 
		(USRCheckChange.socketA_port != param_net_cfg.socketA_port) || 
		(USRCheckChange.socketA_connect_mode != param_net_cfg.socketA_connect_mode) )
	{
		GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		USRGPRS_ReadModbusPrarm();
		if (USR_GRPS_Dev.SocketNetConfig() != USR_ENOERR) {
			GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
	}
}

/**********************************************************************************************************
 @Function			u8* USRGPRS_CheckCmd(u8 *str)
 @Description			���������, �����յ���Ӧ��
 @Input				str		: �ڴ���Ӧ����
 @Return				0		: û�еõ��ڴ���Ӧ����
					����		: �ڴ�Ӧ������λ��(str��λ��)
**********************************************************************************************************/
u8* USRGPRS_CheckCmd(u8 *str)
{
	char *strx=0;
	
	if (USRReceiveLength & 0X8000) {										//���յ�һ��������
		USRReceiveBuf[USRReceiveLength & 0X7FFF] = 0;						//��ӽ�����
		strx = strstr((const char*)USRReceiveBuf, (const char*)str);
	}
	
	return (u8*)strx;
}

/**********************************************************************************************************
 @Function			u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
 @Description			��USR GPRS��������
 @Input				cmd		: ���͵������ַ���
					length	: ���������
					ack		: �ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
					waittime	: �ȴ�ʱ��(��λ:10ms)
 @Return				0		: ���ͳɹ�(�õ����ڴ���Ӧ����)
					1		: ����ʧ��
**********************************************************************************************************/
u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
{
	u8 result = 0;
	
	USR_PortSerialEnable(1, 0);											//���������ж�
	USRReceiveLength = 0;
	memset((void *)USRReceiveBuf, 0x0, sizeof(USRReceiveBuf));
	
	USR_PortSerialSend(cmd, strlen((const char *)cmd));						//��������
	
	if (ack && waittime)												//��Ҫ�ȴ�Ӧ��
	{
		while (--waittime)												//�ȴ�����ʱ
		{
			delay_1ms(10);
			if (USRReceiveLength & 0X8000) {								//���յ��ڴ���Ӧ����
				if (USRGPRS_CheckCmd(ack)) {
					break;
				}
				USRReceiveLength = 0;
			}
		}
		
		if (waittime == 0) {
			result = 1;
		}
	}
	USR_PortSerialEnable(0, 0);											//�رս����ж�
	
	return result;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ModuleInit(void)
 @Description			USR GPRS ��ʼ��ģ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_ModuleInit(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USRInitialized = USR_INITCONFIGING;									//USRGPRS��������
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//��ʼ��USRͨ�Ŵ���
	
	memset(tmpbuf1, 0x0, sizeof(tmpbuf1));
	memset(tmpbuf2, 0x0, sizeof(tmpbuf2));
	sprintf((char *)tmpbuf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpbuf2, "%d", param_net_cfg.socketA_port);
	
	if (USRGPRS_EnterATCmd() != USR_ENOERR) {								//����ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_EchoSet(0) != USR_ENOERR) {									//�رջ���
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_WorkModeSet(3) != USR_ENOERR) {								//����Ϊ����͸��ģʽ
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_CalenSet(0) != USR_ENOERR) {								//�ر�ͨ������
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_NatenSet(0) != USR_ENOERR) {								//�ر�����ATָ��
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_UatenSet(0) != USR_ENOERR) {								//�ر�͸��ģʽ�µĴ���ATָ��
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_RestartTimeSet() != USR_ENOERR) {							//�ر�ģ���Զ�����
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SleepTimeSet() != USR_ENOERR) {								//�ر�ģ���Զ�����͹���
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_REGEnSet(0) != USR_ENOERR) {								//�ر�ע���
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_HEARTEnSet(0) != USR_ENOERR) {								//�ر�������
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASet(0, tmpbuf1, tmpbuf2) != USR_ENOERR) {				//���� SocketA ����
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASLSet(1) != USR_ENOERR) {								//SocketA������
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketAEnSet(1) != USR_ENOERR) {								//ʹ��SocketA
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketBEnSet(0) != USR_ENOERR) {								//�ر�SocketB
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SaveModule() != USR_ENOERR) {								//���浱ǰ����, ģ������
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//�ȴ�����
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS���ý���
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketNetConfig(void)
 @Description			USR GPRS ����Socket���Ӳ���
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketNetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USR_PortSerialEnable(0, 0);											//�رս����ж�
	USRInitialized = USR_INITCONFIGING;									//USRGPRS��������
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//��ʼ��USRͨ�Ŵ���
	
	memset(tmpbuf1, 0x0, sizeof(tmpbuf1));
	memset(tmpbuf2, 0x0, sizeof(tmpbuf2));
	sprintf((char *)tmpbuf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpbuf2, "%d", param_net_cfg.socketA_port);
	
	if (USRGPRS_EnterATCmd() != USR_ENOERR) {								//����ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASet(0, tmpbuf1, tmpbuf2) != USR_ENOERR) {				//���� SocketA ����
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASLSet(1) != USR_ENOERR) {								//SocketA������
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketAEnSet(1) != USR_ENOERR) {								//ʹ��SocketA
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketBEnSet(0) != USR_ENOERR) {								//�ر�SocketB
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SaveModule() != USR_ENOERR) {								//���浱ǰ����, ģ������
		return USR_MODULEINITERR;
	}
	
	delay_1ms(300);													//�ȴ�����
	socket_dev.PortSerialInit(USART3, 115200);								//��ʼ��SocketЭ�鴮��
	TIM_Cmd(TIM2, DISABLE);												//�رն�ʱ��2
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS���ý���
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_EnterATCmd(void)
 @Description			����ATָ��ģʽ
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_EnterATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"+++", (u8 *)"a", 200) == 0) {
		if (USRGPRS_SendCmd((u8 *)"a", (u8 *)"+ok", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ENTERATERR;
		}
	}
	else {
		err = USR_ENTERATERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ExitATCmd(void)
 @Description			�˳�ATָ��ģʽ
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_ExitATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+ENTM\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_RestartModule(void)
 @Description			����ģ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_RestartModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+Z\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_EchoSet(u8 xEnable)
 @Description			���û���
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_EchoSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//��������
		if (USRGPRS_SendCmd((u8 *)"AT+E=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//�رջ���
		if (USRGPRS_SendCmd((u8 *)"AT+E=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_WorkModeSet(u8 mode)
 @Description			���ù���ģʽ
 @Input				mode		   : 1 CMD	ATָ��ģʽ
								2 SMS	����͸��ģʽ
								3 NET	����͸��ģʽ
								4 HTTPD	HTTPDģʽ
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_WorkModeSet(u8 mode)
{
	USRErrorCode err = USR_ENOERR;
	
	if (mode == 1) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"CMD\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 2) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"SMS\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 3) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"NET\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 4) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"HTTPD\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_CalenSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��ͨ������
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_CalenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��ͨ��
		if (USRGPRS_SendCmd((u8 *)"AT+CALEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹͨ��
		if (USRGPRS_SendCmd((u8 *)"AT+CALEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_NatenSet(u8 xEnable)
 @Description			�����Ƿ�ʹ������ATָ��
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_NatenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ������ATָ��
		if (USRGPRS_SendCmd((u8 *)"AT+NATEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹ����ATָ��
		if (USRGPRS_SendCmd((u8 *)"AT+NATEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_UatenSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��͸��ģʽ�µĴ���ATָ��
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_UatenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��͸��ģʽ�µĴ���ATָ��
		if (USRGPRS_SendCmd((u8 *)"AT+UATEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹ͸��ģʽ�µĴ���ATָ��
		if (USRGPRS_SendCmd((u8 *)"AT+UATEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_CachenSet(u8 xEnable)
 @Description			�����Ƿ�ʹ�ܻ�������
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_CachenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ�ܻ�������
		if (USRGPRS_SendCmd((u8 *)"AT+CACHEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹ��������
		if (USRGPRS_SendCmd((u8 *)"AT+CACHEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_RestartTimeSet(void)
 @Description			����ģ���Զ�����ʱ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_RestartTimeSet(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+RSTIM=0\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SleepSet(u8 xEnable)
 @Description			�����ǽ���͹���ģʽ
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SleepSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ�ܵ͹���ģʽ
		if (USRGPRS_SendCmd((u8 *)"AT+SLEEP=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹ�͹���ģʽ
		if (USRGPRS_SendCmd((u8 *)"AT+SLEEP=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SleepTimeSet(void)
 @Description			����ģ���Զ�����͹���ʱ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SleepTimeSet(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+SLEEPTIM=0\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SaveModule(void)
 @Description			���浱ǰ����, ģ������
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SaveModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+S\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ResetUserConfig(void)
 @Description			�ָ��û�Ĭ������, ģ������
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_ResetUserConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+RELD\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ResetConfig(void)
 @Description			�ָ���������, ģ������
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_ResetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+CFGTF\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			���� SocketA ����
 @Input				mode		   : 0 TCP
								1 UDP
					ipaddr	   : ip��ַ
					port		   : �˿�
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrtmpbuf, 0x0, sizeof(usrtmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrtmpbuf, "AT+SOCKA=\"TCP\",\"%s\",%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrtmpbuf, "AT+SOCKA=\"UDP\",\"%s\",%s\r", ipaddr, port);
	}
	
	if (USRGPRS_SendCmd(usrtmpbuf, (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			���� SocketB ����
 @Input				mode		   : 0 TCP
								1 UDP
					ipaddr	   : ip��ַ
					port		   : �˿�
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrtmpbuf, 0x0, sizeof(usrtmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrtmpbuf, "AT+SOCKB=\"TCP\",\"%s\",%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrtmpbuf, "AT+SOCKB=\"UDP\",\"%s\",%s\r", ipaddr, port);
	}
	
	if (USRGPRS_SendCmd(usrtmpbuf, (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��SocketA
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKAEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹSocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKAEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��SocketB
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹSocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketASLSet(u8 type)
 @Description			����SocketA���ӷ�ʽ
 @Input				type		   : 1 long
								0 short
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketASLSet(u8 type)
{
	USRErrorCode err = USR_ENOERR;
	
	if (type == 1) {													//long
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKASL=\"long\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//short
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKASL=\"short\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBSLSet(u8 type)
 @Description			����SocketB���ӷ�ʽ
 @Input				type		   : 1 long
								0 short
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBSLSet(u8 type)
{
	USRErrorCode err = USR_ENOERR;
	
	if (type == 1) {													//long
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBSL=\"long\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//short
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBSL=\"short\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_REGEnSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��ע���
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_REGEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��ע���
		if (USRGPRS_SendCmd((u8 *)"AT+REGEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹע���
		if (USRGPRS_SendCmd((u8 *)"AT+REGEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable)
 @Description			�����Ƿ�ʹ��������
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//ʹ��������
		if (USRGPRS_SendCmd((u8 *)"AT+HEARTEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//��ֹ������
		if (USRGPRS_SendCmd((u8 *)"AT+HEARTEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/********************************************** END OF FLEE **********************************************/
