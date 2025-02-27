#include "bsp_led.h"


void LED1_INIT(void)
{
	GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = LED1_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(LED1_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(LED1_PORT,LED1_PIN);
}

void LED2_INIT(void)
{
	GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	GPIO_InitStructure.Pin        = LED2_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitPeripheral(LED2_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(LED2_PORT,LED2_PIN);
	
}

void LED1_CMD(u8 cmd)
{
	if(cmd == 1)
	{
		GPIO_SetBits(LED1_PORT,LED1_PIN);
	}
	else
	{
		GPIO_ResetBits(LED1_PORT,LED1_PIN);
	}

}

void LED2_CMD(u8 cmd)
{
	if(cmd == 1)
	{
		GPIO_SetBits(LED2_PORT,LED2_PIN);
	}
	else
	{
		GPIO_ResetBits(LED2_PORT,LED2_PIN);
	}

}
void Led2Blink(void)
{
	LED2_PORT->POD ^= LED2_PIN;
}
void Led1Blink(void)
{
	LED1_PORT->POD ^= LED1_PIN;
}






















