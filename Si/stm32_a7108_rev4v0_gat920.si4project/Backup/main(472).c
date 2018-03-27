/**
  *********************************************************************************************************
  * @file    main.c
  * @author  MoveBroad
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *********************************************************************************************************
  */

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "math.h"
#include "NRF24L01.h"
#include "stdarg.h"
#include "string.h"
#include "stm32_config.h"
#include "a7108.h"
#include "a7108reg.h"
#include "mbcrc.h"
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"
#include "mbrtu.h"
#include "rtc.h"
#include "bsp_usart.h"
#include "socketconfig.h"
#include "calculationconfig.h"
#include "iooutputconfig.h"
#include "gatconfig.h"
#include "gatfunc.h"
#include "gatconnect.h"
#include "gatserial.h"


extern u8 ack_geted;
extern int SysTick_count;
extern u8 u8params[RECV_MAX][ACK_LENGTH], u8numx, u8numy, u8numx_now;

u8 a7108_initialized;													//��ʼ��A7108
mvb_param_recv 			param_recv;  									//���������Ĳ���
mvb_param_wvd_config 		param_wvd_cfg; 								//�ش���������
mvb_pkg_wvd_cfg			pkg_wvd_cfg;									//����λ�����͵��������ݰ�
u8 						rssi_value[RECV_MAX];							//�洢��rssiֵ
USHORT 					output_ID[OUTPUT_MAX];							//����˿ڵĲ���
u8 OUTPUT_NUM;          													//����ʹ�õ�����˿�


GPIO_TypeDef* OUTPUT_TYPE[16] =											//IO�������
{
	GPIOA,GPIOA,GPIOA,GPIOA,
	GPIOA,GPIOA,GPIOA,GPIOA,
	GPIOB,GPIOB,GPIOB,GPIOB,
	GPIOB,GPIOB,GPIOB,GPIOB
};

u16 OUTPUT_PIN[16]=
{
	GPIO_Pin_4, GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
	GPIO_Pin_8, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_15,
	GPIO_Pin_0, GPIO_Pin_2,  GPIO_Pin_3,  GPIO_Pin_4,
	GPIO_Pin_6, GPIO_Pin_7,  GPIO_Pin_8,  GPIO_Pin_9
};

/* Private function prototypes -----------------------------------------------*/
USHORT hand_simple_data(void);
USHORT hand_complete_data(void);
void hand_IOOutput(u8* buf);

void init_param_recv_default(u32 sn);
void hand_config(u8* buf);
void param_save_to_flash(void);

extern void SYSCLKConfig_STOP(void);
extern void SetupClock(void);
extern void SetNVIC(void);


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


/* Private define ------------------------------------------------------------*/
#define RO_FLashAddr  	((u32)0x0803E000)
#define RW_StartAddr     ((u32)0x0803F000)
#define RW_FLashAddr_p1  ((u32)0x0803F000)									//����������������
#define RW_FLashAddr_p2  ((u32)0x0803F100)									//�ش����ò���������
#define RW_FLashAddr_p3  ((u32)0x0803F200)									//����˿�ӳ�䱣����
#define EndAddr		((u32)0x0803F400)
#define PageSize		((u16)0x400)

u32 EraseCounter = 0x0;
vu32 NbrOfPage = 0;
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;

void flash_erase(u32 StartAddr)
{
  	FLASHStatus = FLASH_COMPLETE;
  	MemoryProgramStatus = PASSED;
	// Define the number of page to be erased
  	NbrOfPage = (EndAddr - StartAddr) >> 8;
	// Clear All pending flags 
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	// Erase the FLASH pages
  	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  	{
   		FLASHStatus = FLASH_ErasePage(StartAddr+ (PageSize * EraseCounter));
  	}
}
u32 flash_read(u32 addr){
	// Clear All pending flags 
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	return (*(vu32*) addr);
}
u32 flash_write(u32 addr,u32* data,u16 num){
	u16 i = 0;
	// FLASH Word program of data at addr
  	while ((FLASHStatus == FLASH_COMPLETE) && (i<num))
  	{	
    		FLASHStatus = FLASH_ProgramWord((addr+i*4), data[i] );
		i++;
  	}
	return FLASHStatus;
}

void param_save_to_flash(void)
{
	//������д��Flash
	FLASH_Unlock();
	flash_erase(RW_StartAddr);
	flash_write(RW_FLashAddr_p1, (u32*) &param_recv, MVB_PARAM_RECV_BYTE / 4);			//�������ò���
	flash_write(RW_FLashAddr_p2, (u32*) &param_wvd_cfg, MVB_PARAM_WVD_CFG_BYTE / 4+1);	//�������ò���
	flash_write(RW_FLashAddr_p3, (u32*) &output_ID, OUTPUT_MAX / 2);					//�������ò���
	FLASH_Lock();
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u16 rtc_minute_cnt = 0;													//ʱ�Ӷ�ʱ����һ��������ʾһ����
u16 rtc_second_cnt = 0;													//ʱ�Ӷ�ʱ����һ��������ʾһ����
u16 rtc_reset_time = 0;													//����ʱ��ֵ�����趨ʱ����û�յ������������豸
u32 recv_sn;

int main(void)
{
#ifdef DEBUG
	debug();
#endif
	
	int i;
	u32 *u32params;
	u32 temp;
	UCHAR *pFrame ;
	
	SetupClock();														// Configure the system clocks ͬʱʹ������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);		// Enable PWR and BKP clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* STM32��PA.13,PA.14,PA.15,PB.03,PB.04��Ĭ�ϵ�JTAG����ӳ�� */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);					//�ͷ�PB3��PB4��PA15����ͨI/O��
	GPIO_Configuration();												// Configure GPIO
	EXTI_Configuration();												// Configure EXTI
	TIM_Configuration();												// Configure TIMs
	RTC_Time_Init(&systemtime);											// Configure RTC clock source and prescaler
	SetNVIC();														// NVIC configuration
	
	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {
		RCC_ClearFlag();												// Clear reset flags
	} else {
		
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);							//Enable write access to IWDG_PR and IWDG_RLR registers 
	IWDG_SetPrescaler(IWDG_Prescaler_128);									//IWDG counter clock: 40KHz(LSI) / 128 = 312.5 Hz 
	IWDG_SetReload(2500);												//2500 = 8s, debug 80s

	Flag_MASTER = 0;													//��ʼ������
	initRF_a7108();
	IWDG_Enable();														//������ȥ��IWDG

#if 1
	GPIO_SetBits(GPIOC, GPIO_Pin_13); 										//������
	delay_1ms(300);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
#endif

	u8numx = u8numx_now;
	rf_mode = SLEEP_MODE;

	/* �� flash ������ */
	u32params = (u32*) &param_recv;
	FLASH_Unlock();

#ifdef WRITE_SN
	recv_sn = (u32)MAC_SN;
	flash_erase(RO_FLashAddr);
	flash_write(RO_FLashAddr,(u32*) &recv_sn, 1);
#endif

	recv_sn = flash_read(RO_FLashAddr);

	for (i = 0; i < MVB_PARAM_RECV_BYTE / 4; i++) {
		delay_5us(240);	
		u32params[i] = flash_read(RW_FLashAddr_p1 + i * 4);
	}
	u32params  = (u32*) &param_wvd_cfg;
	for (i = 0; i < MVB_PARAM_WVD_CFG_BYTE / 4; i++) {
		delay_5us(240);
		u32params[i] = flash_read(RW_FLashAddr_p2 + i * 4);
	}
	u32params  = (u32*) &output_ID;										//��ȡFlash�д洢��outputID
	for (i = 0; i < OUTPUT_MAX / 2; i++) {
		delay_5us(240);	
		u32params[i] = flash_read(RW_FLashAddr_p3 + i * 4);
	}
	FLASH_Lock();
	socket_dev.ReadOutputID(output_ID);									//�ϵ��ȡoutput_ID����˿ڵĲ�����Socket�������ݰ�
	calculation_dev.ReadOutputID(output_ID);								//�ϵ��ȡoutput_ID����˿ڵĲ�����Calculation�������ݰ�
	iooutput_dev.ReadOutputID(output_ID);									//�ϵ��ȡoutput_ID����˿ڵĲ�����IOOutput�������ݰ�

	OUTPUT_NUM = ((recv_sn / 1000) % 10) * 2;
	if (OUTPUT_NUM > 16) {
	   OUTPUT_NUM = 16;
	}

	for (i = 0; i < IOOutputMAX; i++) {									//ȫ���ص�LED
		GPIO_ResetBits(OUTPUT_TYPE[i], OUTPUT_PIN[i]);
	}

#if 0
	for (i = 0; i < OUTPUT_NUM; i++) {										//����GPIO,ȫ������һ��
		GPIO_SetBits(OUTPUT_TYPE[i], OUTPUT_PIN[i]); 
		delay_1ms(500);
		GPIO_ResetBits(OUTPUT_TYPE[i], OUTPUT_PIN[i]);
		IWDG_ReloadCounter();
	}
#endif

	if (recv_sn == 0xFFFFFFFF) {
		init_param_recv_default(0x12345678);
		param_save_to_flash();
	}
	else if (param_recv.head != recv_sn) {
		init_param_recv_default(recv_sn);
		param_save_to_flash();
	}
	
	temp = recv_sn;													//recv_sn ��ת
	recv_sn = (temp>>24) | ((temp&0xff0000)>>8) | ((temp&0xff00)<<8) | ((temp&0xff)<<24);

#ifdef SOCKET_ENABLE													//ʹ��Socket
	socket_dev.Init();
#endif

#ifdef MODBUS_ENABLE													//ʹ��ModBus
	/* modbus init */
	eMBInit( MB_RTU, param_recv.addr, 0, param_recv.baud, MB_PAR_NONE );
	/* Enable the Modbus Protocol Stack. */
	eMBEnable(  );
	/* �Զ����͵�ַ */
	pFrame =  GetRTUBuf() + 1;
	/* First byte contains the function code. */
	*pFrame++ = MB_FUNC_READ_HOLDING_REGISTER;
	/* Second byte in the response contain the number of bytes. */
	*pFrame++ = 4;
	/* third byte in the response contain the mac. */ 
	*pFrame++ = param_recv.mac_H >> 8;
	*pFrame++ = param_recv.mac_H;
	*pFrame++ = param_recv.mac_L >> 8;
	*pFrame++ = param_recv.mac_L;
	eMBRTUSend(param_recv.addr, GetRTUBuf() + 1, 6);
#endif

	mvb_RX_Mode(ACK_LENGTH);
	
	while (1)
	{
#ifdef MODBUS_ENABLE
		eMBPoll(  );													//��ѯ����ModBus�¼�
#endif
		IWDG_ReloadCounter();											//ι��
	}	 
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
		
	}
}
#endif

//�����ù���IDת��Ϊ1~32
USHORT ID_Convert(USHORT id)
{
	USHORT i;
	for (i = 0; i < OUTPUT_MAX; i++)
	{
		if(output_ID[i] == id)
		{
			return (i+1);
		}
	}
	
	//�����û������
	if(i == OUTPUT_MAX)
		return id;
	return id;
}

/*******************************************************************************
* Function Name  : hand_RTdata
* Description    : ʵʱ�����յ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hand_RTdata(void)
{
	USHORT usLen;

	if (param_recv.simple_mode)
		usLen = hand_simple_data();
	else
		usLen = hand_complete_data();
	
	if (usLen > 6)														//����֡���㹻
		eMBRTUSend(param_recv.addr, GetRTUBuf()+1, usLen);					//�����ý����������滻��ַ
}

//����RFEND���������������ݣ���Ҫ�����򷵻�
void hand_config(u8* buf)
{
	RF_DataHeader_TypeDef* phead = (RF_DataHeader_TypeDef*)buf;
	if(phead->type == DATATYPE_CONFIG_H)
	{
		//�����������ݸ�WVD
		if ((param_wvd_cfg.config_item != 0)&&((param_wvd_cfg.addr_dev == phead->addr_dev)||(param_wvd_cfg.addr_dev == 0xffff))) //�������Ҫ���õĲ���
		{
			mvb_TX_Mode((u8*)&param_wvd_cfg, sizeof(param_wvd_cfg), ACK_LENGTH);
		}
	}
}

//����RFEND�����������ݣ��򵥷���
USHORT hand_simple_data(void)
{
	RF_DataHeader_TypeDef* phead ;
	UCHAR *  pFrame =  GetRTUBuf()+1;
	UCHAR*   pLen =  GetRTUBuf()+2;										//Ӧ���ֽ���
	USHORT usLen = 6;
	/* First byte contains the function code. */
	*pFrame++ = MB_FUNC_READ_HOLDING_REGISTER;
	*pLen  = 4;
	pFrame++;
	//���ƽ�����ID
	memcpy(pFrame, &recv_sn, 4);
	pFrame += 4;
	while(u8numx_now != u8numx)
	{
		phead = (RF_DataHeader_TypeDef*)u8params[u8numx_now];
		if(phead->type != DATATYPE_CONFIG_L)								//ȥ�����ð���һ�η���
		{
			//IDת��
			USHORT id = ID_Convert(phead->addr_dev);
			*pFrame++ = id >> 8;
			*pFrame++ = id;

			//������
			*pFrame++ = u8params[u8numx_now][2];
	
			//������
			*pFrame++ = u8params[u8numx_now][4]; 

			*pLen += 4;
			usLen += 4;
		}
		u8numx_now ++ ;
		u8numx_now %= RECV_MAX;
	}
	return usLen;
}

void hand_IOOutput(u8* buf)
{
	int i;
	u16 carnumstate = 0;
	RF_DataHeader_TypeDef header ;
	
	memcpy(&header, buf, 3);
	
	carnumstate = (((~0xffff) | buf[3])<<8) | buf[4];							//��ȡ������״ֵ̬
	
	//��������,���淽ʽ���
	if ((header.type == DATATYPE_CARIN_WITH_MAGVALUE ) || (header.type  == DATATYPE_CARIN_WITHOUT_MAGVALUE))
	{
		for (i = 0; i < OUTPUT_NUM; i++)
		{
			if ((output_ID[i] == (header.addr_dev)) || (output_ID[i] == 0xFFFF))
			{
				//�������, ����������ʱ��,ʱ�䵽�жϴ���,�Զ��ص�
				if (param_recv.handle_lost != 1) {							//�ж��Ƿ���Ҫ�Զ�������
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
				}
				else {
					iooutput_dev.IOCheck(output_ID[i], carnumstate, 1);		//IO���У�鲹�䶪��
				}
			}
		}
	}
	else if ((header.type  == DATATYPE_CAROUT_WITH_MAGVALUE ) || (header.type  == DATATYPE_CAROUT_WITHOUT_MAGVALUE))
	{
		for (i = 0; i < OUTPUT_NUM; i++)
		{
			if ((output_ID[i] == (header.addr_dev)) || (output_ID[i] == 0xFFFF))
			{
				//�ر����, ����������ʱ��,ʱ�䵽�жϴ���,�Զ��ص�
				if (param_recv.handle_lost != 1) {							//�ж��Ƿ���Ҫ�Զ�������
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
				}
				else {
					iooutput_dev.IOCheck(output_ID[i], carnumstate, 0);		//IO���У�鲹�䶪��
				}
			}
		}
	}
}

//����RFEND������������,��������
USHORT hand_complete_data(void)
{
	RF_DataHeader_TypeDef* phead ;
	UCHAR*  buf;
	UCHAR*  pFrame = GetRTUBuf() + 1;
	UCHAR*  pLen =  GetRTUBuf() + 2;										//Ӧ���ֽ���
	USHORT usLen = 6;
	   
	/* First byte contains the function code. */
	*pFrame++ = MB_FUNC_READ_HOLDING_REGISTER;
	*pLen  = 4;
	pFrame++;
	//���ƽ�����ID
	memcpy(pFrame, &recv_sn, 4);
	pFrame += 4;
	while(u8numx_now != u8numx)
	{
		phead = (RF_DataHeader_TypeDef*)u8params[u8numx_now];	
		buf = u8params[u8numx_now];	
		if(phead->type == DATATYPE_CONFIG_L)
		{															//�����λ���ð�
			memcpy(&pkg_wvd_cfg, phead, 3);
			pkg_wvd_cfg.rssi = rssi_value[u8numx_now];
			pkg_wvd_cfg.p1=  *(u32*)(buf+3);
		}
		else if(phead->type == DATATYPE_CONFIG_H)
		{
			//ƴ�����ݰ�
			if(pkg_wvd_cfg.addr_dev == phead->addr_dev) {
				//IDת��
				pkg_wvd_cfg.addr_dev = ID_Convert(pkg_wvd_cfg.addr_dev);
				*pFrame++ = (pkg_wvd_cfg.addr_dev)  >> 8;
				*pFrame++ = pkg_wvd_cfg.addr_dev ;
				
				pkg_wvd_cfg.rssi =( pkg_wvd_cfg.rssi + rssi_value[u8numx_now]) / 2;		//ȡ2�ε�ƽ��ֵ
				pkg_wvd_cfg.p2= *(u32*)(buf+3);

				memcpy(pFrame, &(pkg_wvd_cfg.sequence_type), 16);				//ֱ�Ӹ���
				pFrame += 14;

				//ԭʼID
				*pFrame++ = (phead->addr_dev)>>8;
				*pFrame++ =  phead->addr_dev;

				*pLen += 18;
				usLen += 18;
			}
		}
		else
		{
			//IDת��
			USHORT id = ID_Convert(phead->addr_dev);
			*pFrame++ = id >> 8;
			*pFrame++ = id;

			//������
			*pFrame++ = u8params[u8numx_now][2];

			//RSSI
			*pFrame++ = rssi_value[u8numx_now];

			//Input Stauts
			memset(pFrame,0,14);										//��0
			//�شű仯��ֵ
			memcpy(pFrame,&u8params[u8numx_now][5],2);
			pFrame += 8;
			//������
			memcpy(pFrame,&u8params[u8numx_now][3],2); 
			pFrame += 4;

			//ԭʼID
			*pFrame++ = (phead->addr_dev)>>8;
			*pFrame++ =  phead->addr_dev;

			*pLen += 18;
			usLen += 18;
		
		}
		u8numx_now ++ ;
		u8numx_now %= RECV_MAX;
	}
	return usLen;
}

void init_param_recv_default(u32 sn)
{
	u32 temp;
	memset(&param_recv,0,24);

  	param_recv.head            		= sn;  
  	param_recv.mac_H           		= sn >> 16;       
  	param_recv.mac_L           		= sn;  
	temp = sn / 10000;
	param_recv.produce_day = temp % 10;
	temp /= 10;
	param_recv.produce_day += (temp % 10) << 4;
	temp /= 10;
	param_recv.produce_day += (temp % 10) << 8;
	temp /= 10;
	param_recv.produce_day += (temp % 10) << 12;
	temp /= 10;
	param_recv.produce_year   = 0x2000 + ((temp / 10) << 4) + (temp % 10);

	param_recv.addr		  		= 1;  
  	param_recv.baud      	  		= 3;
  	param_recv.rt_channel      		= 0;
  	param_recv.init_flag       		= 0;  
  	param_recv.output_mode    		= 0;
  	param_recv.vi_output_high_time 	= 10;
  	param_recv.vo_output_high_time 	= 10;
  	param_recv.output_low_min  		= 5;
  	param_recv.output_high_max 		= 20;
  	param_recv.handle_lost 			= 1;
  	param_recv.check_repeat_time 		= 0;
	param_recv.simple_mode 			= 1;

	param_wvd_cfg.addr_dev 			= 0xffff;
	param_wvd_cfg.config_item 		= 0x0;
	param_wvd_cfg.heartbeat_interval 	= 43200;
	param_wvd_cfg.config_interval 	= 62;
	param_wvd_cfg.work_mode 			= 0xcd;
	param_wvd_cfg.vi_threshhold 		= 30;
	param_wvd_cfg.vo_threshhold 		= 15;
	param_wvd_cfg.measure_mode 		= 0x10;
	param_wvd_cfg.recalibration_time 	= 1;
	param_wvd_cfg.recalibration_value 	= 10;

	memset(output_ID, 0x0, 2 * OUTPUT_MAX);
	socket_dev.ReadOutputID(output_ID);									//��ȡoutput_ID����˿ڵĲ�����Socket�������ݰ�
	calculation_dev.ReadOutputID(output_ID);								//��ȡoutput_ID����˿ڵĲ�����Calculation�������ݰ�
	iooutput_dev.ReadOutputID(output_ID);									//��ȡoutput_ID����˿ڵĲ�����IOOutput�������ݰ�
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	
	if ((( usAddress >= REG_RECV_START ) && ( usAddress + usNRegs <= REG_RECV_START + REG_RECV_NREGS )) ||
	    (( usAddress >= REG_WVD_START ) && ( usAddress + usNRegs <= REG_WVD_START + REG_WVD_NREGS )) )
	{
		switch ( eMode )
		{
		/* Pass current register values to the protocol stack. */
		case MB_REG_READ:
		while( usNRegs > 0 )
		{
			switch ( usAddress)
        		{
				case REG_RW_ADDR:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_recv.addr;
					break;
				case  REG_RW_BAUD:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char ) param_recv.baud;
					break;
				case  REG_RW_RT_CHANNEL:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_recv.rt_channel;
					break;
				case REG_RO_MAC_H :
					*pucRegBuffer++ = ( unsigned char )( param_recv.mac_H>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.mac_H & 0xff);
					break;
				case  REG_RO_MAC_L:
					*pucRegBuffer++ = ( unsigned char )( param_recv.mac_L>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.mac_L & 0xff);
					break;
				case  REG_RO_PRODUCE_YEAR:
					*pucRegBuffer++ = ( unsigned char )( param_recv.produce_year>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.produce_year & 0xff);
					break;		
				case  REG_RO_PRODUCE_DAY:
					*pucRegBuffer++ = ( unsigned char )( param_recv.produce_day>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.produce_day& 0xff);
					break;		
				case  REG_RW_INIT_FLAG:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char ) param_recv.init_flag;
					break;		
				case  REG_RO_PORT_NUM:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = OUTPUT_NUM;
					break;		
				case  REG_RW_OUTPUT_MODE:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_recv.output_mode;
					break;
				case  REG_RW_VI_OUTPUT_HIGH_TIME:
					*pucRegBuffer++ = ( unsigned char )( param_recv.vi_output_high_time>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.vi_output_high_time & 0xff);
					break;
				case  REG_RW_VO_OUTPUT_HIGH_TIME:
					*pucRegBuffer++ = ( unsigned char )( param_recv.vo_output_high_time>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.vo_output_high_time & 0xff);
					break;
				case  REG_RW_OUTPUT_LOW_MIN:
					*pucRegBuffer++ = ( unsigned char )( param_recv.output_low_min>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.output_low_min & 0xff);
					break;
				case  REG_RW_OUTPUT_HIGH_MAX:
					*pucRegBuffer++ = ( unsigned char )( param_recv.output_high_max>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_recv.output_high_max & 0xff);
					break;
				case REG_RW_HANDLE_LOST:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_recv.handle_lost;
					break;
				case  REG_RW_CHECK_REPEAT_TIME:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_recv.check_repeat_time;
					break;
				case  REG_RW_WVD_ID:
					*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.addr_dev>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.addr_dev & 0xff);
					break;
				case  REG_RW_WVD_CONFIG_FLAG:
					*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.config_item>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.config_item & 0xff);
					break;
				case  REG_RW_WVD_WORKMODE:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.work_mode;
					break;
				case  REG_RW_WVD_HEARTBEAT_INTERVAL:
					*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.heartbeat_interval>> 8);
	                		*pucRegBuffer++ = ( unsigned char )( param_wvd_cfg.heartbeat_interval & 0xff);
					break;
				case  REG_RW_WVD_CONFIG_INTERVAL:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.config_interval;
					break;
				case  REG_RW_WVD_VI_THRESHHOLD:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.vi_threshhold;
					break;
				case  REG_RW_WVD_VO_THRESHHOLD:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.vo_threshhold;
					break;
				case  REG_RW_WVD_MEASURE_MODE:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.measure_mode;
					break;
				case  REG_RW_WVD_REBASE_VALUE:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char ) param_wvd_cfg.recalibration_value ;
					break;
				case  REG_RW_WVD_REBASE_TIME:
					*pucRegBuffer++ = 0;
	                		*pucRegBuffer++ = ( unsigned char )param_wvd_cfg.recalibration_time;
					break;
				default:
					*pucRegBuffer++ = 0;
					*pucRegBuffer++ = 0;
			}
			usAddress++;
			usNRegs--;
		}
		break;

            /* Update current register values with new values from the
             * protocol stack. */
		case MB_REG_WRITE:
		while( usNRegs > 0 )
		{
			switch ( usAddress)
			{
				case REG_RW_ADDR:
					pucRegBuffer++;
	                		param_recv.addr = *pucRegBuffer++;							
					eMBDisable(  );
  					eMBInit( MB_RTU, param_recv.addr, 0, param_recv.baud, MB_PAR_NONE );
					eMBEnable(  );	
					break;
				case  REG_RW_BAUD:
					pucRegBuffer++;
	                		param_recv.baud = *pucRegBuffer++;
					eMBDisable(  );
  					eMBInit( MB_RTU, param_recv.addr, 0, param_recv.baud, MB_PAR_NONE );
					eMBEnable(  );
					break;
				case  REG_RW_RT_CHANNEL:
					pucRegBuffer++;
	                		param_recv.rt_channel = *pucRegBuffer++;
					break;			
				case  REG_RW_INIT_FLAG:
					pucRegBuffer++;
	                		param_recv.init_flag = *pucRegBuffer++;
					break;			
				case  REG_RW_OUTPUT_MODE:
					pucRegBuffer++;
	                		param_recv.output_mode = *pucRegBuffer++;
					break;
				case  REG_RW_VI_OUTPUT_HIGH_TIME:
					param_recv.vi_output_high_time = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_recv.vi_output_high_time |= *pucRegBuffer++;
					break;
				case  REG_RW_VO_OUTPUT_HIGH_TIME:
					param_recv.vo_output_high_time = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_recv.vo_output_high_time |= *pucRegBuffer++;
					break;
				case  REG_RW_OUTPUT_LOW_MIN:
					param_recv.output_low_min = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_recv.output_low_min |= *pucRegBuffer++;
					break;
				case  REG_RW_OUTPUT_HIGH_MAX:
					param_recv.output_high_max = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_recv.output_high_max |= *pucRegBuffer++;
					break;
				case REG_RW_HANDLE_LOST:
					pucRegBuffer++;
	                		param_recv.handle_lost = *pucRegBuffer++;
					break;
				case  REG_RW_CHECK_REPEAT_TIME:
					pucRegBuffer++;
	                		param_recv.check_repeat_time = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_ID:
					param_wvd_cfg.addr_dev = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_wvd_cfg.addr_dev |= *pucRegBuffer++;
					break;
				case  REG_RW_WVD_CONFIG_FLAG:
					param_wvd_cfg.config_item = *pucRegBuffer << 8;
					pucRegBuffer++;
						param_wvd_cfg.config_item|= *pucRegBuffer++;
					break;
				case  REG_RW_WVD_WORKMODE:
					pucRegBuffer++;
	                		param_wvd_cfg.work_mode = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_HEARTBEAT_INTERVAL:
					param_wvd_cfg.heartbeat_interval = *pucRegBuffer << 8;
					pucRegBuffer++;
	                		param_wvd_cfg.heartbeat_interval |= *pucRegBuffer++;
					break;
				case  REG_RW_WVD_CONFIG_INTERVAL:
					pucRegBuffer++;
	                		param_wvd_cfg.config_interval = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_VI_THRESHHOLD:
					pucRegBuffer++;
	                		param_wvd_cfg.vi_threshhold = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_VO_THRESHHOLD:
					pucRegBuffer++;
	                		param_wvd_cfg.vo_threshhold = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_MEASURE_MODE:
					pucRegBuffer++;
	                		param_wvd_cfg.measure_mode = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_REBASE_VALUE:
					pucRegBuffer++;
	                		param_wvd_cfg.recalibration_value = *pucRegBuffer++;
					break;
				case  REG_RW_WVD_REBASE_TIME:
					pucRegBuffer++;
	                		param_wvd_cfg.recalibration_time = *pucRegBuffer++;
					break;
				default:pucRegBuffer +=2;

				//���������λ����24Сʱ���Զ����
				if(param_wvd_cfg.config_item != 0)
					rtc_minute_cnt = 0;				
				}
				usAddress++;
				usNRegs--;
			}
			param_save_to_flash();
			socket_dev.ReadOutputID(output_ID);							//��ȡoutput_ID����˿ڵĲ�����Socket�������ݰ�
			calculation_dev.ReadOutputID(output_ID);						//��ȡoutput_ID����˿ڵĲ�����Calculation�������ݰ�
			iooutput_dev.ReadOutputID(output_ID);							//��ȡoutput_ID����˿ڵĲ�����IOOutput�������ݰ�
		}
	}
	else if (( usAddress >= REG_OUTPUT_START ) && ( usAddress + usNRegs <= REG_OUTPUT_START + OUTPUT_MAX))
	{
    	    iRegIndex = ( int )( usAddress - REG_OUTPUT_START);
          switch ( eMode )
		{
		/* Pass current register values to the protocol stack. */
		case MB_REG_READ:
			while( usNRegs > 0 )
            		{
                		*pucRegBuffer++ = ( unsigned char )( output_ID[iRegIndex] >> 8);
                		*pucRegBuffer++ = ( unsigned char )( output_ID[iRegIndex] & 0xFF);
               		iRegIndex++;
                		usNRegs--;
            		}
            break;
		/* Update current register values with new values from the protocol stack. */
		case MB_REG_WRITE:
			while ( usNRegs > 0 )
			{
				output_ID[iRegIndex] = *pucRegBuffer++<<8;
				output_ID[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
			param_save_to_flash();
			socket_dev.ReadOutputID(output_ID);							//��ȡoutput_ID����˿ڵĲ�����Socket�������ݰ�
			calculation_dev.ReadOutputID(output_ID);						//��ȡoutput_ID����˿ڵĲ�����Calculation�������ݰ�
			iooutput_dev.ReadOutputID(output_ID);							//��ȡoutput_ID����˿ڵĲ�����IOOutput�������ݰ�
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	return MB_ENOREG;
}

/********************************************** END OF FLEE **********************************************/
