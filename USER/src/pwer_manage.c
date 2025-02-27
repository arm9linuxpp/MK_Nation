#include "power_manage.h"


void Before_Stop2_Cfg(void)
{
	taskENTER_CRITICAL(); 
	
	GPIO_InitType GPIO_InitStructure;
	
	//???
	
	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);
  GPIO_ResetBits(NAD_PWR_PORT,NAD_PWR_PIN);
  GPIO_ResetBits(NAD_WRITE_DISBLE_PORT,NAD_WRITE_DISBLE_PIN); 
	LED1_CMD(0);
	LED2_CMD(0);
	//???
	GPIO_InitStructure.Pin        = NAD_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;    
  GPIO_InitPeripheral(NAD_STATE_PORT, &GPIO_InitStructure);
//	
	GPIO_InitStructure.Pin        = NAD_NET_STATUS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(NAD_NET_STATUS_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = NAD_NET_STATUS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(NAD_AP_READY_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = MCU_AISC_INT_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(MCU_AISC_INT_PORT, &GPIO_InitStructure);
	
	
	//USART1_Send_String("AT+QPOWD\r\n");
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
	GPIO_ConfigPinRemap(GPIO_RMP_PD01,ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
	
	//??jtag ????????
	GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_SW_ENABLE,ENABLE);  //??200uA
	GPIO_InitStructure.Pin        = GPIO_PIN_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_14;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_4;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	
//	USART_DeInit(USART1);														
//	USART_DeInit(DEBUG_USARTx);
//	ADC_DeInit(ADC2);
//	I2C_DeInit(I2C2);
	
  SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;			    //??????? 
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;

  taskEXIT_CRITICAL();
}

void After_Stop2_Cfg(void)
{
	GPIO_InitType GPIO_InitStructure;
	SetSysClockToHSI();
	SetSysClockToPLL(72000000, SYSCLK_PLLSRC_HSI);
//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			    //???????
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;	
//	USART_DeInit(USART1);															//?Deinit ????,????????
//	USART1_INIT();																		//??1???
//	USART_DeInit(DEBUG_USARTx);
//	DEBUG_INIT();																			//Debug ??? 
	KeyWakeupInit();																	//???????
	LED1_INIT();																			//LED???
	LED2_INIT();
	//GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_DISABLE,DISABLE);
//	ADC2_Init();
//	Aisc_Int();
//	BG77_GPIO_Init();
//	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);	
	//BG77_Power_On();
  
}












