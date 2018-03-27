/**
  *********************************************************************************************************
  * @file    usrK2.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR-K2 Super Port��������
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrK2.h"
#include "usrserial.h"
#include "string.h"
#include "stdio.h"
#include "socketconfig.h"


extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];
extern void delay_1ms(u32 nCount);
extern mvb_param_net_config		param_net_cfg;									//�������ò���

u8 usrk2tmpbuf[USR_CACHE_SIZE];
u8 tmpk2buf1[20];
u8 tmpk2buf2[20];
u8 tmpk2buf3[20];


/**********************************************************************************************************
 @Function			void USRK2_ReadModbusPrarm(void)
 @Description			��ȡ����Modbus����
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRK2_ReadModbusPrarm(void)
{
	USRCheckChange.device_addr_h = param_net_cfg.device_addr_h;
	USRCheckChange.device_addr_l = param_net_cfg.device_addr_l;
	USRCheckChange.device_mask_h = param_net_cfg.device_mask_h;
	USRCheckChange.device_mask_l = param_net_cfg.device_mask_l;
	USRCheckChange.device_bcast_h = param_net_cfg.device_bcast_h;
	USRCheckChange.device_bcast_l = param_net_cfg.device_bcast_l;
	
	USRCheckChange.socketA_addr_h = param_net_cfg.socketA_addr_h;
	USRCheckChange.socketA_addr_l = param_net_cfg.socketA_addr_l;
	USRCheckChange.socketA_port = param_net_cfg.socketA_port;
	USRCheckChange.socketA_connect_mode = param_net_cfg.socketA_connect_mode;
}

/**********************************************************************************************************
 @Function			void USRGPRS_MonitorChange(void)
 @Description			����K2�����Ƿ�ı�
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRK2_MonitorChange(void)
{
	if ( (USRCheckChange.device_addr_h != param_net_cfg.device_addr_h) || 
		(USRCheckChange.device_addr_l != param_net_cfg.device_addr_l) || 
		(USRCheckChange.device_mask_h != param_net_cfg.device_mask_h) || 
		(USRCheckChange.device_mask_l != param_net_cfg.device_mask_l) || 
		(USRCheckChange.device_bcast_h != param_net_cfg.device_bcast_h) || 
		(USRCheckChange.device_bcast_l != param_net_cfg.device_bcast_l) || 
		(USRCheckChange.socketA_addr_h != param_net_cfg.socketA_addr_h) || 
		(USRCheckChange.socketA_addr_l != param_net_cfg.socketA_addr_l) || 
		(USRCheckChange.socketA_port != param_net_cfg.socketA_port) || 
		(USRCheckChange.socketA_connect_mode != param_net_cfg.socketA_connect_mode) )
	{
		GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		USRK2_ReadModbusPrarm();
		if (USR_K2_Dev.SocketNetConfig() != USR_ENOERR) {
			GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
	}
}

/**********************************************************************************************************
 @Function			u8* USRK2_CheckCmd(u8 *str)
 @Description			���������, �����յ���Ӧ��
 @Input				str		: �ڴ���Ӧ����
 @Return				0		: û�еõ��ڴ���Ӧ����
					����		: �ڴ�Ӧ������λ��(str��λ��)
**********************************************************************************************************/
u8* USRK2_CheckCmd(u8 *str)
{
	char *strx=0;
	
	if (USRReceiveLength & 0X8000) {										//���յ�һ��������
		USRReceiveBuf[USRReceiveLength & 0X7FFF] = 0;						//��ӽ�����
		strx = strstr((const char*)USRReceiveBuf, (const char*)str);
	}
	
	return (u8*)strx;
}

/**********************************************************************************************************
 @Function			u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
 @Description			��USR K2��������
 @Input				cmd		: ���͵������ַ���
					length	: ���������
					ack		: �ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
					waittime	: �ȴ�ʱ��(��λ:10ms)
 @Return				0		: ���ͳɹ�(�õ����ڴ���Ӧ����)
					1		: ����ʧ��
**********************************************************************************************************/
u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
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
				if (USRK2_CheckCmd(ack)) {
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
 @Function			USRErrorCode USRK2_ModuleInit(void)
 @Description			USR K2 ��ʼ��ģ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_ModuleInit(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USRInitialized = USR_INITCONFIGING;									//USRK2��������
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//��ʼ��USRͨ�Ŵ���
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	memset(tmpk2buf3, 0x0, sizeof(tmpk2buf3));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.device_addr_h/256, param_net_cfg.device_addr_h%256, param_net_cfg.device_addr_l/256, param_net_cfg.device_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d.%d.%d.%d", param_net_cfg.device_mask_h/256, param_net_cfg.device_mask_h%256, param_net_cfg.device_mask_l/256, param_net_cfg.device_mask_l%256);
	sprintf((char *)tmpk2buf3, "%d.%d.%d.%d", param_net_cfg.device_bcast_h/256, param_net_cfg.device_bcast_h%256, param_net_cfg.device_bcast_l/256, param_net_cfg.device_bcast_l%256);
	
	if (USRK2_EnterATCmd() != USR_ENOERR) {									//����ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	if (USRK2_EchoSet(0) != USR_ENOERR) {									//�رջ���
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RFCENSet(1) != USR_ENOERR) {									//ʹ����RFC2217
		return USR_MODULEINITERR;
	}
	
	if (USRK2_CLIENTRSTSet(1) != USR_ENOERR) {								//ʹ�� TCP Client ��η���ʧ�� reset ����
		return USR_MODULEINITERR;
	}
	
	if (USRK2_WANNSet(DHCP, tmpk2buf1, tmpk2buf2, tmpk2buf3) != USR_ENOERR) {		//���� WANN ����
		return USR_MODULEINITERR;
	}
	
	if (USRK2_SocketPortSet(LOCALPORTMODE, (u8 *)LOCALPORT) != USR_ENOERR) {		//���ñ��ض˿ں�
		return USR_MODULEINITERR;
	}
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d", param_net_cfg.socketA_port);
	
	if (USRK2_SocketSet(1, tmpk2buf1, tmpk2buf2) != USR_ENOERR) {				//���� Socket ����
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RestartModule() != USR_ENOERR) {								//�����˳�ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//�ȴ�����
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS���ý���
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketNetConfig(void)
 @Description			USR K2 ����Socket���Ӳ���
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_SocketNetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USR_PortSerialEnable(0, 0);											//�رս����ж�
	USRInitialized = USR_INITCONFIGING;									//USRGPRS��������
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//��ʼ��USRͨ�Ŵ���
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	memset(tmpk2buf3, 0x0, sizeof(tmpk2buf3));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.device_addr_h/256, param_net_cfg.device_addr_h%256, param_net_cfg.device_addr_l/256, param_net_cfg.device_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d.%d.%d.%d", param_net_cfg.device_mask_h/256, param_net_cfg.device_mask_h%256, param_net_cfg.device_mask_l/256, param_net_cfg.device_mask_l%256);
	sprintf((char *)tmpk2buf3, "%d.%d.%d.%d", param_net_cfg.device_bcast_h/256, param_net_cfg.device_bcast_h%256, param_net_cfg.device_bcast_l/256, param_net_cfg.device_bcast_l%256);

	if (USRK2_EnterATCmd() != USR_ENOERR) {									//����ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	if (USRK2_WANNSet(DHCP, tmpk2buf1, tmpk2buf2, tmpk2buf3) != USR_ENOERR) {		//���� WANN ����
		return USR_MODULEINITERR;
	}
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d", param_net_cfg.socketA_port);
	
	if (USRK2_SocketSet(1, tmpk2buf1, tmpk2buf2) != USR_ENOERR) {				//���� Socket ����
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RestartModule() != USR_ENOERR) {								//�����˳�ATָ��ģʽ
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//�ȴ�����
	socket_dev.PortSerialInit(USART3, 115200);								//��ʼ��SocketЭ�鴮��
	TIM_Cmd(TIM2, DISABLE);												//�رն�ʱ��2
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS���ý���
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_EnterATCmd(void)
 @Description			����ATָ��ģʽ
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_EnterATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"+++", (u8 *)"a", 200) == 0) {
		if (USRK2_SendCmd((u8 *)"a", (u8 *)"+ok", 200) == 0) {
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
 @Function			USRErrorCode USRK2_ExitATCmd(void)
 @Description			�˳�ATָ��ģʽ
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_ExitATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+ENTM\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RestartModule(void)
 @Description			����ģ��
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_RestartModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+Z\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_EchoSet(u8 xEnable)
 @Description			���û���
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_EchoSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//��������
		if (USRK2_SendCmd((u8 *)"AT+E=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//�رջ���
		if (USRK2_SendCmd((u8 *)"AT+E=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RestoreSetting(void)
 @Description			�ָ���������
 @Input				void
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_RestoreSetting(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+RELD\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway)
 @Description			���� WANN ����
 @Input				mode		   : 0 static
								1 DHCP
					address	   : IP��ַ
					mask		   : ��������
					gateway	   : ���ص�ַ
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrk2tmpbuf, "AT+WANN=static,%s,%s,%s\r", address, mask, gateway);
		
		if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		if (USRK2_SendCmd((u8 *)"AT+WANN=DHCP\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			���� Socket ����
 @Input				mode		   : 0 TCP Server
								1 TCP Client 
								2 UDP Server
								3 UDP Client
								4 Httpd Client
					ipaddr	   : ip��ַ
					port		   : �˿�
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPS,%s,%s\r", ipaddr, port);
	}
	else if (mode == 1) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPC,%s,%s\r", ipaddr, port);
	}
	else if (mode == 2) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=UDPS,%s,%s\r", ipaddr, port);
	}
	else if (mode == 3) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=UDPC,%s,%s\r", ipaddr, port);
	}
	else if (mode == 4) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=HTPC,%s,%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPC,%s,%s\r", ipaddr, port);
	}
	
	if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port)
 @Description			���ñ��� Socket �˿ں�
 @Input				mode		   : 0 ����˿�
								1 ���ö˿�
					port		   : �˿�
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		if (USRK2_SendCmd((u8 *)"AT+SOCKPORT=0\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		sprintf((char *)usrk2tmpbuf, "AT+SOCKPORT=%s\r", port);
		
		if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RFCENSet(u8 xEnable)
 @Description			ʹ��/��ֹ��RFC2217
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_RFCENSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//����RFC2217
		if (USRK2_SendCmd((u8 *)"AT+RFCEN=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//�ر�RFC2217
		if (USRK2_SendCmd((u8 *)"AT+RFCEN=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable)
 @Description			ʹ��/��ֹ TCP Client ��η���ʧ�� reset ����
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : ������Ϣ
**********************************************************************************************************/
USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//����CLIENTRST
		if (USRK2_SendCmd((u8 *)"AT+CLIENTRST=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//�ر�CLIENTRST
		if (USRK2_SendCmd((u8 *)"AT+CLIENTRST=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/********************************************** END OF FLEE **********************************************/
