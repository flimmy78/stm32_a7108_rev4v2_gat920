#ifndef __LESTC_FUNC_H
#define   __LESTC_FUNC_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "lestcconfig.h"

unsigned char LestcGetCheckCode(LestcPacketDataTypeDef* PacketData);				//��ȡУ��ֵ
void LestcCarInSetStatus(LestcPacketDataTypeDef* PacketData, unsigned char status);	//��ȡ����ʻ����Ϣ
void LestcCarOutSetStatus(LestcPacketDataTypeDef* PacketData, unsigned char status);	//��ȡ����ʻ����Ϣ

#endif
