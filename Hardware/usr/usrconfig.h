#ifndef __USR_CONFIG_H
#define   __USR_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"

extern mvb_param_net_config USRCheckChange;										//����Ƿ���Ҫ����GPRS/RJ45

typedef enum
{
	USR_ENOERR,						//No Error
	USR_EPORTINITERR,					//Serial Port Initialized Error
	USR_ENTERATERR,					//EnterATCmd Error
	USR_ATEXECUTEERR,					//AT Cmd Execute Error
	USR_MODULEINITERR,					//Module Initialized Error
	USR_NETCONFIGERR					//Net Config Error
}USRErrorCode;

typedef enum
{
	USR_INITCONFIGOVER,					//USR���ý���
	USR_INITCONFIGING					//USR������
}USRInitializedType;					//USR����������ͣSocketͨ��
extern USRInitializedType USRInitialized;


/* USRGPRS�ⲿ�ӿڿ��ƹ����� */
struct _m_usrgprs_dev
{
	USRErrorCode	(*EnterATCmd)(void);										//����ATָ��ģʽ
	USRErrorCode	(*ExitATCmd)(void);											//�˳�ATָ��ģʽ
	USRErrorCode	(*RestartModule)(void);										//����ģ��
	USRErrorCode	(*EchoSet)(u8 xEnable);										//���û���
	USRErrorCode	(*WorkModeSet)(u8 mode);										//���ù���ģʽ
	USRErrorCode	(*CalenSet)(u8 xEnable);										//�����Ƿ�ʹ��ͨ������
	USRErrorCode	(*NatenSet)(u8 xEnable);										//�����Ƿ�ʹ������ATָ��
	USRErrorCode	(*UatenSet)(u8 xEnable);										//�����Ƿ�ʹ��͸��ģʽ�µĴ���ATָ��
	USRErrorCode	(*CachenSet)(u8 xEnable);									//�����Ƿ�ʹ�ܻ�������
	USRErrorCode	(*RestartTimeSet)(void);										//����ģ���Զ�����ʱ��
	USRErrorCode	(*SleepSet)(u8 xEnable);										//�����ǽ���͹���ģʽ
	USRErrorCode	(*SleepTimeSet)(void);										//����ģ���Զ�����͹���ʱ��
	USRErrorCode	(*SaveModule)(void);										//���浱ǰ����, ģ������
	USRErrorCode	(*ResetUserConfig)(void);									//�ָ��û�Ĭ������, ģ������
	USRErrorCode	(*ResetConfig)(void);										//�ָ���������, ģ������
	USRErrorCode	(*SocketASet)(u8 mode, u8 *ipaddr, u8 *port);					//���� SocketA ����
	USRErrorCode	(*SocketBSet)(u8 mode, u8 *ipaddr, u8 *port);					//���� SocketB ����
	USRErrorCode	(*SocketAEnSet)(u8 xEnable);									//�����Ƿ�ʹ��SocketA
	USRErrorCode	(*SocketBEnSet)(u8 xEnable);									//�����Ƿ�ʹ��SocketB
	USRErrorCode	(*SocketASLSet)(u8 type);									//����SocketA���ӷ�ʽ
	USRErrorCode	(*SocketBSLSet)(u8 type);									//����SocketB���ӷ�ʽ
	USRErrorCode	(*REGEnSet)(u8 xEnable);										//�����Ƿ�ʹ��ע���
	USRErrorCode	(*HEARTEnSet)(u8 xEnable);									//�����Ƿ�ʹ��������
	
	USRErrorCode	(*ModuleInit)(void);										//USR GPRS ��ʼ��ģ��
	USRErrorCode	(*SocketNetConfig)(void);									//USR GPRS ����Socket���Ӳ���
	
	void			(*ReadModbusPrarm)(void);									//��ȡ����Modbus����
	void			(*MonitorChange)(void);										//����GPRS�����Ƿ�ı�
};
extern struct _m_usrgprs_dev USR_GRPS_Dev;

/* USRK2�ⲿ�ӿڿ��ƹ����� */
struct _m_usrk2_dev
{
	USRErrorCode	(*EnterATCmd)(void);										//����ATָ��ģʽ
	USRErrorCode	(*ExitATCmd)(void);											//�˳�ATָ��ģʽ
	USRErrorCode	(*RestartModule)(void);										//����ģ��
	USRErrorCode	(*EchoSet)(u8 xEnable);										//���û���
	USRErrorCode	(*RestoreSetting)(void);										//�ָ���������
	USRErrorCode	(*WANNSet)(u8 mode, u8 *address, u8 *mask, u8 *gateway);			//���� WANN ����
	USRErrorCode	(*SocketSet)(u8 mode, u8 *ipaddr, u8 *port);						//���� Socket ����
	USRErrorCode	(*SocketPortSet)(u8 mode, u8 *port);							//���ñ��ض˿ں�
	USRErrorCode	(*RFCENSet)(u8 xEnable);										//ʹ��/��ֹ��RFC2217
	USRErrorCode	(*CLIENTRSTSet)(u8 xEnable);									//ʹ��/��ֹ TCP Client ��η���ʧ�� reset ����
	
	USRErrorCode	(*ModuleInit)(void);										//USR K2 ��ʼ��ģ��
	USRErrorCode	(*SocketNetConfig)(void);									//USR K2 ����Socket���Ӳ���
	
	void			(*ReadModbusPrarm)(void);									//��ȡ����Modbus����
	void			(*MonitorChange)(void);										//����K2�����Ƿ�ı�
};
extern struct _m_usrk2_dev USR_K2_Dev;

#endif
