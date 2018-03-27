#ifndef _BYTE_DEF_
#define _BYTE_DEF_

#include "stm32f10x_lib.h"
#include "platform_config.h"
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */

				

#define READ_REG1        0x00  // Define read command to register
#define WRITE_REG1       0x20  // Define write command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61   // Define RX payload register address
#define WR_TX_PLOAD     0xA0   // Define TX payload register address
#define FLUSH_TX        0xE1   // Define flush TX register command
#define FLUSH_RX        0xE2   // Define flush RX register command
#define REUSE_TX_PL     0xE3   // Define reuse TX payload register command
#define NOP             0xFF   // Define No Operation, might be used to read status register

//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00   // 'Config' register address
#define EN_AA           0x01   // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02   // 'Enabled RX addresses' register address
#define SETUP_AW        0x03   // 'Setup address width' register address
#define SETUP_RETR      0x04   // 'Setup Auto. Retrans' register address
#define RF_CH           0x05   // 'RF channel' register address
#define RF_SETUP        0x06   // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

#ifdef NRF_GLOBALS 
#define NRF_EXT
#else
#define NRF_EXT extern 
#endif

#define Led_ON()   GPIO_SetBits(GPIOB, GPIO_Pin_5);  	   //LED_ON
#define Led_OFF()  GPIO_ResetBits(GPIOB, GPIO_Pin_5); 	   //LED_OFF

#define SPI_RW_Reg_M 		0x01
#define SPI_Read_M   		0x02
#define SPI_Write_Buf_M 0x03
#define SPI_Read_Buf_M 	0x04
#define TX_MODE_M 	0x05
#define A7108_SLEEP_M		0x06
#define RX_MODE_M 	0x07

#define uchar unsigned char
#define TX_ADR_WIDTH    5 
#define TX_PLOAD_WIDTH  32 
NRF_EXT unsigned char it_msg,led_flash;




extern uchar RX_ADDRESS0[TX_ADR_WIDTH];
extern uchar RX_ADDRESS1[TX_ADR_WIDTH];
extern uchar RX_ADDRESS2[1];
extern uchar RX_ADDRESS3[1];
extern uchar RX_ADDRESS4[1];
extern uchar RX_ADDRESS5[1];
extern uchar RX_ADDRESSES[RECV_MAX][14];//0~4 5~10,11,12,13,14
NRF_EXT unsigned char rx_buf[TX_PLOAD_WIDTH];
NRF_EXT unsigned char tx_buf[TX_PLOAD_WIDTH];
NRF_EXT unsigned char status_buf[TX_PLOAD_WIDTH];
NRF_EXT unsigned char flag,nrf_baud,nrf_Pipe,nrf_Pipe_r;

extern u8 spi_reg,spi_data,*spi_data_p,spi_data_num,func_num,spi_status;//120706


extern void delay_ms(unsigned int x);

extern void delay_10us(unsigned int x);
extern void nrf24L01_IST(void);





