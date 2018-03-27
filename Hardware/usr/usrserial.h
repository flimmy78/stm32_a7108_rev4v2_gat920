#ifndef __USR_SERIAL_H
#define   __USR_SERIAL_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


#define USR_ENTER_CRITICAL_SECTION()		USR_EnterCriticalSection()
#define USR_EXIT_CRITICAL_SECTION()		USR_ExitCriticalSection()

#define USR_CACHE_SIZE		64													//���ݻ�������С

typedef enum
{
	USR_PAR_NONE,																/* !< No parity.   */
	USR_PAR_ODD,																/* !< Odd parity.  */
	USR_PAR_EVEN																/* !< Even parity. */
}USRParity;

extern volatile u8  USRReceiveBuf[USR_CACHE_SIZE];										//USR�������ݻ�����
extern volatile u16 USRReceiveLength;													//�������ݳ���

void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);										//ʹ��/ʧ�� ����/���� �����ж�
USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity);	//��ʼ������
void USR_PortSerialPutByte(u8 ucByte);													//���ڷ��� 1Byte ����
void USR_PortSerialGetByte(u8 *pucByte);												//���ڽ��� 1Byte ����
void USR_PortSerialSend(u8 *data_arr, u32 length);										//���ڷ�������

void USR_UARTx_IRQ(USART_TypeDef* USARTx);												//USR�����жϴ�����
void USR_FrameCBByteReceived(void);													//USR�����жϽ��մ�����
void USR_FrameCBTransmitterEmpty(void);													//USR�����жϷ��ʹ�����

void USR_EnterCriticalSection(void);												//����ؼ�����,�ر��ж�,�ȴ��жϽ�������
void USR_ExitCriticalSection(void);												//�˳��ؼ�����,�����ж�

#endif
