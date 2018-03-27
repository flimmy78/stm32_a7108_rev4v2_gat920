/**
  *********************************************************************************************************
  * @file    usrconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR����ת���ڶ����ⲿ���ýӿ��ļ�
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrconfig.h"
#include "usrgprs.h"
#include "usrk2.h"
#include "usrserial.h"


USRInitializedType USRInitialized = USR_INITCONFIGOVER;						//USR���ñ��(����������ͣSocketͨ��), Ĭ��û����USR
mvb_param_net_config USRCheckChange;										//����Ƿ���Ҫ����GPRS/RJ45


/* gat920�ⲿ�ӿ����������� */
struct _m_usrgprs_dev USR_GRPS_Dev = 
{
	USRGPRS_EnterATCmd,													//����ATָ��ģʽ
	USRGPRS_ExitATCmd,													//�˳�ATָ��ģʽ
	USRGPRS_RestartModule,												//����ģ��
	USRGPRS_EchoSet,													//���û���
	USRGPRS_WorkModeSet,												//���ù���ģʽ
	USRGPRS_CalenSet,													//�����Ƿ�ʹ��ͨ������
	USRGPRS_NatenSet,													//�����Ƿ�ʹ������ATָ��
	USRGPRS_UatenSet,													//�����Ƿ�ʹ��͸��ģʽ�µĴ���ATָ��
	USRGPRS_CachenSet,													//�����Ƿ�ʹ�ܻ�������
	USRGPRS_RestartTimeSet,												//����ģ���Զ�����ʱ��
	USRGPRS_SleepSet,													//�����ǽ���͹���ģʽ
	USRGPRS_SleepTimeSet,												//����ģ���Զ�����͹���ʱ��
	USRGPRS_SaveModule,													//���浱ǰ����, ģ������
	USRGPRS_ResetUserConfig,												//�ָ��û�Ĭ������, ģ������
	USRGPRS_ResetConfig,												//�ָ���������, ģ������
	USRGPRS_SocketASet,													//���� SocketA ����
	USRGPRS_SocketBSet,													//���� SocketB ����
	USRGPRS_SocketAEnSet,												//�����Ƿ�ʹ��SocketA
	USRGPRS_SocketBEnSet,												//�����Ƿ�ʹ��SocketB
	USRGPRS_SocketASLSet,												//����SocketA���ӷ�ʽ
	USRGPRS_SocketBSLSet,												//����SocketB���ӷ�ʽ
	USRGPRS_REGEnSet,													//�����Ƿ�ʹ��ע���
	USRGPRS_HEARTEnSet,													//�����Ƿ�ʹ��������
	
	USRGPRS_ModuleInit,													//USR GPRS ��ʼ��ģ��
	USRGPRS_SocketNetConfig,												//USR GPRS ����Socket���Ӳ���
	
	USRGPRS_ReadModbusPrarm,												//��ȡ����Modbus����
	USRGPRS_MonitorChange,												//����GPRS�����Ƿ�ı�
};

/* gat920�ⲿ�ӿ����������� */
struct _m_usrk2_dev USR_K2_Dev = 
{
	USRK2_EnterATCmd,													//����ATָ��ģʽ
	USRK2_ExitATCmd,													//�˳�ATָ��ģʽ
	USRK2_RestartModule,												//����ģ��
	USRK2_EchoSet,														//���û���
	USRK2_RestoreSetting,												//�ָ���������
	USRK2_WANNSet,														//���� WANN ����
	USRK2_SocketSet,													//���� Socket ����
	USRK2_SocketPortSet,												//���ñ��ض˿ں�
	USRK2_RFCENSet,													//ʹ��/��ֹ��RFC2217
	USRK2_CLIENTRSTSet,													//ʹ��/��ֹ TCP Client ��η���ʧ�� reset ����
	
	USRK2_ModuleInit,													//USR K2 ��ʼ��ģ��
	USRK2_SocketNetConfig,												//USR K2 ����Socket���Ӳ���
	
	USRK2_ReadModbusPrarm,												//��ȡ����Modbus����
	USRK2_MonitorChange,												//����K2�����Ƿ�ı�
};

/********************************************** END OF FLEE **********************************************/
