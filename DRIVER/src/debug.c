#include "debug.h"
#include "stdio.h"
#include "main.h"

static void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = DEBUG_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void DEBUG_INIT(void)
{
		GPIO_InitType GPIO_InitStructure;
		USART_InitType USART_InitStructure;
			
	  RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5,ENABLE);
		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA|RCC_APB2_PERIPH_GPIOB|RCC_APB2_PERIPH_GPIOC|RCC_APB2_PERIPH_AFIO,ENABLE);
	  GPIO_ConfigPinRemap(GPIO_RMP3_UART5,ENABLE);
    /* Configure USARTy Rx as input floating */
    GPIO_InitStructure.Pin       = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitPeripheral(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.Pin        = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
		 /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
    USART_Init(DEBUG_USARTx, &USART_InitStructure);
		NVIC_Configuration();
    USART_ConfigInt(DEBUG_USARTx, USART_INT_RXDNE, ENABLE);
    USART_Enable(DEBUG_USARTx, ENABLE);
}

static int is_lr_sent = 0;
int fputc(int ch, FILE* f)
{
    if (ch == '\r')
    {
        is_lr_sent = 1;
    }
    else if (ch == '\n')
    {
        if (!is_lr_sent)
        {
            USART_SendData(DEBUG_USARTx, (uint8_t)'\r');
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXDE) == RESET)
            {
            }
        }
        is_lr_sent = 0;
    }
    else
    {
        is_lr_sent = 0;
    }
    USART_SendData(DEBUG_USARTx, (uint8_t)ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXDE) == RESET)
    {
    }
    return ch;
}

void DebugSend1Byte(u8 data) 
{	
	USART_SendData(DEBUG_USARTx,data);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXDE) == RESET)
	{}	
}

void Debug_Send_String(char *str,u8 len)
{
  int i;
	for(i=0;i<len;i++)
	{
	  USART_SendData(DEBUG_USARTx,str[i]);
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXDE) == RESET)
		{
		}
	}
}
