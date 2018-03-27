/**
  *********************************************************************************************************
  * @file    gatserial.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *********************************************************************************************************
  */

#include "gatserial.h"


#ifdef GAT920_SERIALPORT_USART1
#define GAT920_USART	USART1
#endif
#ifdef GAT920_SERIALPORT_USART2
#define GAT920_USART	USART2
#endif
#ifdef GAT920_SERIALPORT_USART3
#define GAT920_USART	USART3
#endif

volatile u8  GATReceiveBuf[GAT_CACHE_SIZE];									//GAT�������ݻ�����
volatile u8  GATSendBuf[GAT_CACHE_SIZE];									//GAT�������ݻ�����
volatile u16 GATReceiveLength;											//GAT�������ݳ���    [15] : 0,û�н��յ�����; 1,���յ���һ֡����
volatile u16 GATSendLength;												//GAT�������ݳ���    [15] : 0,�������ڷ���;   1,���ݷ������
volatile u16 GATTxCounter;												//GAT��������ָ��

/**********************************************************************************************************
 @Function			void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			ʹ��/ʧ�� ����/���� �����ж�
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	GAT_ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//ʹ�ܴ��ڽ����ж�
		while (USART_GetFlagStatus(GAT920_USART, USART_FLAG_TC) == 0);			//�ȴ����������һ���ֽڳ��ٿ�����
#ifdef GAT920_SERIALPORT_USART1
		GPIO_ResetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
#ifdef GAT920_SERIALPORT_USART2
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#endif
#ifdef GAT920_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
#endif
	}
	else {															//ʧ�ܴ��ڽ����ж�
		USART_ITConfig(GAT920_USART, USART_IT_RXNE, DISABLE);
	}
	
	if (xTxEnable) {													//ʹ�ܴ��ڷ����ж�
#ifdef GAT920_SERIALPORT_USART1
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		GPIO_SetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
#endif
#ifdef GAT920_SERIALPORT_USART2
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#endif
#ifdef GAT920_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
#endif
	}
	else {
		USART_ITConfig(GAT920_USART, USART_IT_TXE, DISABLE);					//ʧ�ܴ��ڷ����ж�
	}
	
	GAT_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity)
 @Description			��ʼ��GAT����
 @Input				Port		: ���ں�
					BaudRate	: ������
					DataBits	: ����λ 7/8
					Parity	: У��λ GAT_PAR_NONE/GAT_PAR_ODD/GAT_PAR_EVEN
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity)
{
	u8 Initialized = GAT_TRUE;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* USART Clock / GPIO Config */
	if (Port == USART1) {
		/* Config USART1 and GPIOA Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO Config */
		/* Config USART1 TX ( PA.9 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART1 RX ( PA.10 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART1_RS485 DE ( PA.1 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if (Port == USART2) {
		/* Config USART2 and GPIOA Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART2 GPIO Config */
		/* Config USART2 TX ( PA.2 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART2 RX ( PA.3 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if (Port == USART3) {
		/* Config USART3 and GPIOB Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		/* USART3 GPIO Config */
		/* Config USART3 TX ( PB.10 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		/* Config USART3 RX ( PB.11 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else {
		Initialized = GAT_FALSE;
	}
	
	/* USART MODE Config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	
	switch (DataBits)
	{
	case 8:
		if (Parity == GAT_PAR_NONE)
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		Initialized = GAT_FALSE;
	}
	
	switch (Parity)
	{
	case GAT_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case GAT_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case GAT_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	
	if (Initialized == GAT_TRUE) {
		GAT_ENTER_CRITICAL_SECTION();
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(GAT920_USART, &USART_InitStructure);
		USART_Cmd(GAT920_USART, ENABLE);
		
		GAT_EXIT_CRITICAL_SECTION();
	}
	
	return Initialized;
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialPutByte(u8 ucByte)
 @Description			���ڷ��� 1Byte����
 @Input				ucByte	: 1Byte����
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_PortSerialPutByte(u8 ucByte)
{
	USART_SendData(GAT920_USART, ucByte);
	while (USART_GetFlagStatus(GAT920_USART, USART_FLAG_TXE) == 0);				//�ȴ����ͼĴ�����
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialGetByte(u8 *pucByte)
 @Description			���ڽ��� 1Byte����
 @Input				pucByte	: ����1Byte���ݵ�ַ
 @Return				GAT_TRUE  : ��ȷ
					GAT_FALSE : ����
**********************************************************************************************************/
u8 GAT_PortSerialGetByte(u8 *pucByte)
{
	*pucByte = (u8)USART_ReceiveData(GAT920_USART);							//��ȡ���յ�������
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			void GAT_UARTx_IRQ(USART_TypeDef* USARTx)
 @Description			GATЭ�鴮���жϴ�����
 @Input				USARTx : ���ں�
 @Return				void
**********************************************************************************************************/
void GAT_UARTx_IRQ(USART_TypeDef* USARTx)
{
	if (USART_GetITStatus(USARTx, USART_IT_TXE)) {							//�����жϴ���
		if (USARTx == GAT920_USART) {
			GAT_FrameCBTransmitterEmpty();								//�����жϴ�����
		}
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);						//�巢���жϱ�־λ
	}
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {						//�����жϴ���
		if (USARTx == GAT920_USART) {
			GAT_FrameCBByteReceived();									//�����жϴ�����
		}
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);						//������жϱ�־λ
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBByteReceived(void)
 @Description			GATЭ�鴮���жϽ��մ�����
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_FrameCBByteReceived(void)
{
	u8 recvivedata;
	
	GAT_PortSerialGetByte((u8 *) &recvivedata);								//��ȡ���ڽ��յ�����
	if ((GATReceiveLength & (1<<15)) == 0)									//�������һ������,��û�б�����,���ٽ�����������
	{
		if (GATReceiveLength < GAT_CACHE_SIZE) {							//�����Խ�������
			TIM_SetCounter(TIM1, 0);										//��ʱ���������
			if (GATReceiveLength == 0) {									//ʹ�ܶ�ʱ��1���ж�
				TIM_Cmd(TIM1, ENABLE);									//ʹ�ܶ�ʱ��1
			}
			GATReceiveBuf[GATReceiveLength++] = recvivedata;					//��¼���յ���ֵ
		}
		else {
			GATReceiveLength |= 1<<15;									//ǿ�Ʊ�ǽ������
		}
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBTransmitterEmpty(void)
 @Description			GATЭ�鴮���жϷ��ʹ�����
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_FrameCBTransmitterEmpty(void)
{
	GAT_PortSerialPutByte(GATSendBuf[GATTxCounter++]);						//��������
	if ((GATTxCounter == GATSendLength) || (GATTxCounter >= GAT_CACHE_SIZE)) {		//�����������
		USART_ITConfig(GAT920_USART, USART_IT_TXE, DISABLE);					//�رշ����ж�
		GATTxCounter = 0;
		GATSendLength |= 1<<15;											//��־�����������
	}
}















/**********************************************************************************************************
 @Function			void GAT_EnterCriticalSection(void)
 @Description			����ؼ�����,�ر��ж�,�ȴ��жϽ�������
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void GAT_ExitCriticalSection(void)
 @Description			�˳��ؼ�����,�����ж�
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_ExitCriticalSection(void)
{
	__enable_irq();
}

/********************************************** END OF FLEE **********************************************/
