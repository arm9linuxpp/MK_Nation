#include "bsp_uart.h"
#include "n32g45x.h"
#include "stdio.h"

u8  RxBuf[200];				
u16 RxCount;
u8  IDLE_FLAG; 						

static void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannel            = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void USART1_INIT(void)
{
		GPIO_InitType GPIO_InitStructure;
		USART_InitType USART_InitStructure;
	  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA|RCC_APB2_PERIPH_AFIO|RCC_APB2_PERIPH_USART1,ENABLE);
	  	
    GPIO_InitStructure.Pin       = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
    GPIO_InitStructure.Pin        = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
    USART_Init(USART1, &USART_InitStructure);
		NVIC_Configuration();
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
		USART_ConfigInt(USART1, USART_INT_IDLEF, ENABLE);
    USART_Enable(USART1, ENABLE);
		
}


// uart Rx Pin wakeup config
void Uart_Exti_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
	NVIC_InitType NVIC_InitStructure;
	EXTI_InitType EXTI_InitStructure;
	
  USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
	USART_ConfigInt(USART1, USART_INT_IDLEF, DISABLE);
  USART_Enable(USART1, DISABLE);
	
	USART_DeInit(USART1);
	
	GPIO_InitStructure.Pin       = GPIO_PIN_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_ConfigEXTILine(GPIOA_PORT_SOURCE, GPIO_PIN_SOURCE10);
	EXTI_InitStructure.EXTI_Line = EXTI_LINE10;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitPeripheral(&EXTI_InitStructure);
	/*Set key input interrupt priority*/
	NVIC_InitStructure.NVIC_IRQChannel                   = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void USART1Send1Byte(u8 data) 
{	
	USART_SendData(USART1,data);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXDE) == RESET)
	{}	
}

void USART1_Send_String(char *s)
{
	while(*s)
	{
		USART1Send1Byte(*s++);
	}
}

void USART1_Send_Len(const char *data, int txLen)
{
	int i =0;
	for(i=0;i<txLen;i++)
	{
	  USART1Send1Byte(data[i]);
	}
}

