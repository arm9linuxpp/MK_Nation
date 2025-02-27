#ifndef __BSP_UART_H__
#define __BSP_UART_H__
#include "main.h"

extern u8  RxBuf[200];
extern u16 RxCount;
extern u8  IDLE_FLAG;



void USART1_INIT(void);
void Uart_Exti_Init(void);
void USART1Send1Byte(u8 data);
void USART1_Send_String(char *s);
void USART1_Send_Len(const char *data, int txLen);
#endif


