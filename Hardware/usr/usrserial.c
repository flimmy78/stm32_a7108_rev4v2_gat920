/**
  *********************************************************************************************************
  * @file    usrserial.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR����ͨ��Э��
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrserial.h"


#define USRSerialPort		USART3										//USR����

volatile u8  USRReceiveBuf[USR_CACHE_SIZE];									//USR�������ݻ�����
volatile u16 USRReceiveLength;											//USR�������ݳ���    [15] : 0,û�н��յ�����; 1,���յ���һ֡����


extern void Delay(vu32 nCount);
#define RS485_Mode_RX()		GPIO_ResetBits( GPIOA, GPIO_Pin_1 )
#define RS485_Mode_TX()		GPIO_SetBits( GPIOA, GPIO_Pin_1 )

/**********************************************************************************************************
 @Function			void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			ʹ��/ʧ�� ����/���� �����ж�
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	USR_ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//ʹ�ܴ��ڽ����ж�
		while (USART_GetFlagStatus(USRSerialPort, USART_FLAG_TC) == 0);			//�ȴ����������һ���ֽڳ��ٿ�����
		if (USRSerialPort == USART1) {
			GPIO_ResetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		}
		USART_ITConfig(USRSerialPort, USART_IT_RXNE, ENABLE);
	}
	else {															//ʧ�ܴ��ڽ����ж�
		USART_ITConfig(USRSerialPort, USART_IT_RXNE, DISABLE);
	}
	
	if (xTxEnable) {													//ʹ�ܴ��ڷ����ж�
		USART_ITConfig(USRSerialPort, USART_IT_TXE, ENABLE);
		if (USRSerialPort == USART1) {
			GPIO_SetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		}
	}
	else {															//ʧ�ܴ��ڷ����ж�
		USART_ITConfig(USRSerialPort, USART_IT_TXE, DISABLE);
	}
	
	USR_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity)
 @Description			��ʼ��USR����
 @Input				Port			: ���ں�
					BaudRate		: ������
					DataBits		: ����λ 7/8
					Parity		: У��λ GAT_PAR_NONE/GAT_PAR_ODD/GAT_PAR_EVEN
 @Return				USRErrorCode	: ������Ϣ
**********************************************************************************************************/
USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity)
{
	USRErrorCode Initialized = USR_ENOERR;
	
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
		Initialized = USR_EPORTINITERR;
	}
	
	/* USART MODE Config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	
	switch (DataBits)
	{
	case 8:
		if (Parity == USR_PAR_NONE)
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		Initialized = USR_EPORTINITERR;
	}
	
	switch (Parity)
	{
	case USR_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case USR_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case USR_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	
	if (Initialized == USR_ENOERR) {
		USR_ENTER_CRITICAL_SECTION();
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(Port, &USART_InitStructure);
		USART_Cmd(Port, ENABLE);
		
		USR_EXIT_CRITICAL_SECTION();
	}
	
	return Initialized;
}

/**********************************************************************************************************
 @Function			void USR_PortSerialPutByte(u8 ucByte)
 @Description			���ڷ��� 1Byte ����
 @Input				ucByte	: 1Byte����
 @Return				void
**********************************************************************************************************/
void USR_PortSerialPutByte(u8 ucByte)
{
	USART_SendData(USRSerialPort, ucByte);
	while (USART_GetFlagStatus(USRSerialPort, USART_FLAG_TXE) == 0);				//�ȴ����ͼĴ�����
}

/**********************************************************************************************************
 @Function			void USR_PortSerialGetByte(u8 *pucByte)
 @Description			���ڽ��� 1Byte ����
 @Input				pucByte	: ����1Byte���ݵ�ַ
 @Return				void
**********************************************************************************************************/
void USR_PortSerialGetByte(u8 *pucByte)
{
	*pucByte = (u8)USART_ReceiveData(USRSerialPort);							//��ȡ���յ�������
}

/**********************************************************************************************************
 @Function			void USR_PortSerialSend(u8 *data_arr, u32 length)
 @Description			���ڷ�������
 @Input				data_arr	: ���ݵ�ַ
					length	: ���ݳ���
 @Return				void
**********************************************************************************************************/
void USR_PortSerialSend(u8 *data_arr, u32 length)
{
	if (USRSerialPort == USART1) {
		RS485_Mode_TX();
		Delay(0x1fff);
		while (length > 0) {
			USR_PortSerialPutByte(*data_arr++);
			length--;
		}
		Delay(0x1fff);
		RS485_Mode_RX();
	}
	else {
		while (length > 0) {
			USR_PortSerialPutByte(*data_arr++);
			length--;
		}
	}
}

/**********************************************************************************************************
 @Function			void USR_UARTx_IRQ(USART_TypeDef* USARTx)
 @Description			USR�����жϴ�����
 @Input				USARTx : ���ں�
 @Return				void
**********************************************************************************************************/
void USR_UARTx_IRQ(USART_TypeDef* USARTx)
{
	if (USART_GetITStatus(USARTx, USART_IT_TXE)) {							//�����жϴ���
		if (USARTx == USRSerialPort) {
			USR_FrameCBTransmitterEmpty();								//�����жϴ�����
		}
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);						//�巢���жϱ�־λ
	}
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {						//�����жϴ���
		if (USARTx == USRSerialPort) {
			USR_FrameCBByteReceived();									//�����жϴ�����
		}
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);						//������жϱ�־λ
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBByteReceived(void)
 @Description			USR�����жϽ��մ�����
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_FrameCBByteReceived(void)
{
	u8 recvivedata;
	
	USR_PortSerialGetByte((u8 *) &recvivedata);								//��ȡ���ڽ��յ�����
	if ((USRReceiveLength & (1<<15)) == 0)									//�������һ������,��û�б�����,���ٽ�����������
	{
		if (USRReceiveLength < USR_CACHE_SIZE) {							//�����Խ�������
			TIM_SetCounter(TIM2, 0);										//��ʱ���������
			if (USRReceiveLength == 0) {									//ʹ�ܶ�ʱ��2���ж�
				TIM_Cmd(TIM2, ENABLE);									//ʹ�ܶ�ʱ��2
			}
			USRReceiveBuf[USRReceiveLength++] = recvivedata;					//��¼���յ���ֵ
		}
		else {
			USRReceiveLength |= 1<<15;									//ǿ�Ʊ�ǽ������
		}
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBTransmitterEmpty(void)
 @Description			USR�����жϷ��ʹ�����
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_FrameCBTransmitterEmpty(void)
{
	
}

/**********************************************************************************************************
 @Function			void USR_EnterCriticalSection(void)
 @Description			����ؼ�����,�ر��ж�,�ȴ��жϽ�������
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void USR_ExitCriticalSection(void)
 @Description			�˳��ؼ�����,�����ж�
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_ExitCriticalSection(void)
{
	__enable_irq();
}

/********************************************** END OF FLEE **********************************************/
