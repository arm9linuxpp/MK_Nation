#include "aisc.h"

u8 Aisc_RxBuf[256];  //接收缓冲
u8 Aisc_RxCnt;			
u8 Aisc_RxEnd;			 //接收完成标志


void Aisc_BP_High(void)
{
  GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = MCU_AISC_INT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(MCU_AISC_INT_PORT, &GPIO_InitStructure);	
}

void Aisc_BP_low(void)
{
  GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = MCU_AISC_INT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(MCU_AISC_INT_PORT, &GPIO_InitStructure);	
	GPIO_ResetBits(MCU_AISC_INT_PORT,MCU_AISC_INT_PIN);
}
void Aisc_Init(void)
{
  Aisc_BP_High();
	i2c_slave_init();
}

