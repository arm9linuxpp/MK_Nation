#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "n32g45x.h"
#define log_info(...) printf(__VA_ARGS__)

//时钟开启 没有宏定义，更改需要去.c文件
#define  DEBUG_USARTx                   UART5
#define  DEBUG_USART_BAUDRATE           115200  
#define  DEBUG_USART_TX_GPIO_PORT       GPIOB 
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_8
#define  DEBUG_USART_RX_GPIO_PORT       GPIOB
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_9

#define  DEBUG_USART_IRQ                UART5_IRQn
#define  DEBUG_USART_IRQHandler         UART5_IRQHandler

void DEBUG_INIT(void);
void Debug_Send_String(char *str,u8 len);
void DebugSend1Byte(u8 data);
#endif

