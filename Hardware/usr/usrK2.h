#ifndef __USR_K2_H
#define   __USR_K2_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


u8* USRK2_CheckCmd(u8 *str);												//���������, �����յ���Ӧ��
u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime);								//��USR K2��������


void USRK2_ReadModbusPrarm(void);											//��ȡ����Modbus����
void USRK2_MonitorChange(void);											//����K2�����Ƿ�ı�

USRErrorCode USRK2_ModuleInit(void);										//USR K2 ��ʼ��ģ��
USRErrorCode USRK2_SocketNetConfig(void);									//USR K2 ����Socket���Ӳ���


USRErrorCode USRK2_EnterATCmd(void);										//����ATָ��ģʽ
USRErrorCode USRK2_ExitATCmd(void);										//�˳�ATָ��ģʽ
USRErrorCode USRK2_RestartModule(void);										//����ģ��
USRErrorCode USRK2_EchoSet(u8 xEnable);										//���û���
USRErrorCode USRK2_RestoreSetting(void);									//�ָ���������
USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway);			//���� WANN ����
USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port);					//���� Socket ����
USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port);							//���ñ��ض˿ں�
USRErrorCode USRK2_RFCENSet(u8 xEnable);									//ʹ��/��ֹ��RFC2217
USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable);									//ʹ��/��ֹ TCP Client ��η���ʧ�� reset ����

#endif
