#ifndef __USR_GPRS_H
#define   __USR_GPRS_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


u8* USRGPRS_CheckCmd(u8 *str);											//���������, �����յ���Ӧ��
u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime);							//��USR GPRS��������


USRErrorCode USRGPRS_ModuleInit(void);										//USR GPRS ��ʼ��ģ��
USRErrorCode USRGPRS_SocketNetConfig(void);									//USR GPRS ����Socket���Ӳ���

void USRGPRS_ReadModbusPrarm(void);										//��ȡ����Modbus����
void USRGPRS_MonitorChange(void);											//����GPRS�����Ƿ�ı�


USRErrorCode USRGPRS_EnterATCmd(void);										//����ATָ��ģʽ
USRErrorCode USRGPRS_ExitATCmd(void);										//�˳�ATָ��ģʽ
USRErrorCode USRGPRS_RestartModule(void);									//����ģ��
USRErrorCode USRGPRS_EchoSet(u8 xEnable);									//���û���
USRErrorCode USRGPRS_WorkModeSet(u8 mode);									//���ù���ģʽ
USRErrorCode USRGPRS_CalenSet(u8 xEnable);									//�����Ƿ�ʹ��ͨ������
USRErrorCode USRGPRS_NatenSet(u8 xEnable);									//�����Ƿ�ʹ������ATָ��
USRErrorCode USRGPRS_UatenSet(u8 xEnable);									//�����Ƿ�ʹ��͸��ģʽ�µĴ���ATָ��
USRErrorCode USRGPRS_CachenSet(u8 xEnable);									//�����Ƿ�ʹ�ܻ�������
USRErrorCode USRGPRS_RestartTimeSet(void);									//����ģ���Զ�����ʱ��
USRErrorCode USRGPRS_SleepSet(u8 xEnable);									//�����ǽ���͹���ģʽ
USRErrorCode USRGPRS_SleepTimeSet(void);									//����ģ���Զ�����͹���ʱ��
USRErrorCode USRGPRS_SaveModule(void);										//���浱ǰ����, ģ������
USRErrorCode USRGPRS_ResetUserConfig(void);									//�ָ��û�Ĭ������, ģ������
USRErrorCode USRGPRS_ResetConfig(void);										//�ָ���������, ģ������
USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port);					//���� SocketA ����
USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port);					//���� SocketB ����
USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable);								//�����Ƿ�ʹ��SocketA
USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable);								//�����Ƿ�ʹ��SocketB
USRErrorCode USRGPRS_SocketASLSet(u8 type);									//����SocketA���ӷ�ʽ
USRErrorCode USRGPRS_SocketBSLSet(u8 type);									//����SocketB���ӷ�ʽ
USRErrorCode USRGPRS_REGEnSet(u8 xEnable);									//�����Ƿ�ʹ��ע���
USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable);									//�����Ƿ�ʹ��������

#endif
