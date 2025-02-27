#ifndef __AISC_H__
#define __AISC_H__
#include "bsp_iic.h"
#include "n32g45x.h"
#include "main.h"

#define MCU_IIC_SLAVE_ADDR						0x010       //mcu 作为IIC从机的地址
#define MCU_AISC_INT_PORT							GPIOB			
#define MCU_AISC_INT_PIN							GPIO_PIN_1	 


extern u8 Aisc_RxBuf[256];
extern u8 Aisc_RxCnt;
extern u8 Aisc_RxEnd;

void Aisc_Init(void);
void Aisc_BP_low(void);
void Aisc_BP_High(void);
#endif
































