#include <stdio.h>
#include "main.h"
TaskHandle_t LED1Task_Handler ;
TaskHandle_t LED2Task_Handler ;
TaskHandle_t AiscTask_Handler;

//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]__attribute__ ((at(0X20022000)));

void led1_task(void *pvParameters);
void led2_task(void *pvParameters);
void Aisc_Task(void);

void Before_Stop2_Cfg(void);
void After_Stop2_Cfg(void);
int main(void)
{
	//SCB->VTOR = 0x08000000|0x10000;			// bootloader  
	SetSysClockToHSI();
	SetSysClockToPLL(72000000, SYSCLK_PLLSRC_HSI);
	USART1_INIT();
	DEBUG_INIT();
	DumpClock("HSI");
	LED1_INIT();
	LED2_INIT();
	KeyWakeupInit();
  SysTick_Init();
	rtc_Init();
	ADC2_Init();	
	BG77_GPIO_Init();
	Aisc_Init();
	BG77_Power_On();
	delay_ms(2000);

	
	taskENTER_CRITICAL(); 
	xTaskCreate((TaskFunction_t )led1_task,     	
                (const char*    )"led1_task",   	
                (uint16_t       )80, 
                (void*          )NULL,				
                (UBaseType_t    )3,	
                (TaskHandle_t*  )&LED1Task_Handler);
  xTaskCreate((TaskFunction_t )led2_task,     	
                (const char*    )"led2_task",   	
                (uint16_t       )80, 
                (void*          )NULL,				
                (UBaseType_t    )2,	
                (TaskHandle_t*  )&LED2Task_Handler); 
	xTaskCreate((TaskFunction_t )Aisc_Task,     	
                (const char*    )"Aisc_Task",   	
                (uint16_t       )100, 
                (void*          )NULL,				
                (UBaseType_t    )1,	
                (TaskHandle_t*  )&AiscTask_Handler); 
  taskEXIT_CRITICAL();
								
	memset(RxBuf,0,RxCount);  //clean bg77 started send "app rdy"
	RxCount = 0;	
  vTaskStartScheduler();
}

void led1_task(void *pvParameters)
{
  while(1)
	{
		LED1_CMD(1);
		vTaskDelay(200);
	}
}

void led2_task(void *pvParameters)
{
  while(1)
	{		
		vTaskDelay(10000);
	}

}

void Aisc_Task(void)
{	
	while(1)
	{
		vTaskDelay(20);	  
	}
  
}


void StartStandbyeMode(void)
{
	if (PWR_GetFlagStatus(PWR_WU_FLAG) != RESET)
	{
		/* Clear Wake Up flag */
		PWR_ClearFlag(PWR_WU_FLAG);
	}
	log_info("\r\n start standby \r\n");
  PWR_EnterStandbyState();
}


void Before_Stop2_Cfg(void)
{
	taskENTER_CRITICAL(); 
	
	GPIO_InitType GPIO_InitStructure;
	
	//输入脚
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
	
	USART1_Send_String("AT\r\n");
	delay_ms(20);
  USART1_Send_String("AT+QPOWD\r\n");
	delay_ms(20);

	GPIO_InitStructure.Pin        = MCU_AISC_INT_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(MCU_AISC_INT_PORT, &GPIO_InitStructure);
	
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
	GPIO_ConfigPinRemap(GPIO_RMP_PD01,ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
	
	//关闭jtag 并设置相应上下拉
	GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_SW_ENABLE,ENABLE);  //下降200uA
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
	
	
  USART_Enable(USART1, DISABLE);
	USART_Enable(DEBUG_USARTx, DISABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_9;  //usart1 rx
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_8;  //usart1 tx
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_9;  //debug rx
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.Pin        = GPIO_PIN_8;  //debug tx
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	//关闭相关外设
//	ADC_DeInit(ADC2);
//	I2C_DeInit(I2C2);
	
  SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;			    //关闭滴答定时器 
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;

//输出脚	
	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);
  GPIO_ResetBits(NAD_PWR_PORT,NAD_PWR_PIN);
  GPIO_ResetBits(NAD_WRITE_DISBLE_PORT,NAD_WRITE_DISBLE_PIN); 
  LED1_CMD(0);
	LED2_CMD(0);
	
  taskEXIT_CRITICAL();
}

void After_Stop2_Cfg(void)
{
	GPIO_InitType GPIO_InitStructure;
	SetSysClockToHSI();
	SetSysClockToPLL(72000000, SYSCLK_PLLSRC_HSI);
//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			    //开启滴答定时器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;	
	USART_DeInit(USART1);															//先Deinit 再初始化，否则会坏一个字节
	USART1_INIT();																		//串口1初始化
//	USART_DeInit(DEBUG_USARTx);
	DEBUG_INIT();																			//Debug 初始化 
	KeyWakeupInit();																	//按键唤醒初始化
	LED1_INIT();																			//LED初始化
	LED2_INIT();
//	ADC2_Init();
//	Aisc_Int();
//	BG77_GPIO_Init();
//	GPIO_ResetBits(NAD_RST_PORT,NAD_RST_PIN);	
	//BG77_Power_On();
  
}

void delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
		    
		 //i=25000;					//144M:25000
		 i=15000;					//72M:15000
		 //i=2000;				//8M
      while(i--) ;    
   }
 }
void delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=120;  
      while(i--) ;    
   }
}


extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{	
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();	
    }
}

void SysTick_Init(void)
{
	u32 reload;

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	reload = (SystemCoreClock / 1000000);					
	reload *= (1000000 / configTICK_RATE_HZ);       
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			  
	SysTick->LOAD=reload; 					     					  
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 			

}


#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param file pointer to the source file name
 * @param line assert_param error line source number
 */
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif

/**
 * @}
 */

/**
 * @}
 */
