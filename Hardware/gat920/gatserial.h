#ifndef __GAT_SERIAL_H
#define   __GAT_SERIAL_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


#define GAT_ENTER_CRITICAL_SECTION()		GAT_EnterCriticalSection()
#define GAT_EXIT_CRITICAL_SECTION()		GAT_ExitCriticalSection()

typedef enum
{
	GAT_PAR_NONE,																/* !< No parity.   */
	GAT_PAR_ODD,																/* !< Odd parity.  */
	GAT_PAR_EVEN																/* !< Even parity. */
}GATParity;

extern volatile u8  GATReceiveBuf[GAT_CACHE_SIZE];									//GAT�������ݻ�����
extern volatile u8  GATSendBuf[GAT_CACHE_SIZE];										//GAT�������ݻ�����
extern volatile u16 GATReceiveLength;												//GAT�������ݳ���
extern volatile u16 GATSendLength;													//GAT�������ݳ���


void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);									//ʹ��/ʧ�� ����/���� �����ж�
u8   GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity);	//��ʼ������
u8   GAT_PortSerialPutByte(u8 ucByte);												//���ڷ��� 1Byte����
u8   GAT_PortSerialGetByte(u8 *pucByte);											//���ڽ��� 1Byte����

void GAT_UARTx_IRQ(USART_TypeDef* USARTx);											//GATЭ�鴮���жϴ�����
void GAT_FrameCBByteReceived(void);												//GATЭ�鴮���жϽ��մ�����
void GAT_FrameCBTransmitterEmpty(void);												//GATЭ�鴮���жϷ��ʹ�����

void GAT_EnterCriticalSection(void);												//����ؼ�����,�ر��ж�,�ȴ��жϽ�������
void GAT_ExitCriticalSection(void);												//�˳��ؼ�����,�����ж�

#endif
