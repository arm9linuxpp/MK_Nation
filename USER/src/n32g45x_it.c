/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g45x_it.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "n32g45x_it.h"
#include "main.h"
#include "bsp_uart.h"
#include "debug.h"
#include "aisc.h"
//#include "cellular_comm_interface.h"
#include "bsp_iic.h"
/** @addtogroup N32G45X_StdPeriph_Template
 * @{
 */

extern uint8_t TxBuffer1[];
extern uint8_t TxBuffer2[];
extern uint8_t RxBuffer1[];
extern uint8_t RxBuffer2[];
extern __IO uint8_t TxCounter1;
extern __IO uint8_t TxCounter2;
extern __IO uint8_t RxCounter1;
extern __IO uint8_t RxCounter2;
extern uint8_t NbrOfDataToTransfer1;
extern uint8_t NbrOfDataToTransfer2;
extern uint8_t NbrOfDataToRead1;
extern uint8_t NbrOfDataToRead2;

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
	printf("HardFault_Handler\n");
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
			
			//LED2_CMD(1);
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
//void SVC_Handler(void)
//{
//}

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void)
{
}
/******************************************************************************/
/*                 N32G45X Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32g45x.s).                                                 */
/******************************************************************************/

void USART1_IRQHandler(void)
{
	u8 data;
	if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET)
  {
        //RxBuf[RxCount++] = USART_ReceiveData(USART1);
		    data = USART_ReceiveData(USART1);
		    RxBuf[RxCount++] = data;
		    //USART_SendData(DEBUG_USARTx,data);  // 测试at用
		
				if(RxCount>65534) RxCount = 0;
				USART_ClrIntPendingBit(USART1,USART_INT_RXDNE);
		    
   }
	if (USART_GetIntStatus(USART1, USART_INT_IDLEF) != RESET)
	{	
		USART_ClrIntPendingBit(USART1,USART_INT_IDLEF);
		USART1->STS;
		USART1->DAT;
		//IDLE_FLAG = 1;
		
		BG77.rev_end = 0;
		memset(BG77.RxBuf,0,BG77.RxCnt); //清除上次发送AT接收到的缓冲
		BG77.RxCnt = 0;
//		if(cellular_rev_start_f == 1)   //表示这组数据是cellular 需要的。
//		{
//		  memcpy(cellular_buf,RxBuf,RxCount);
//			cellularRxCount = RxCount;
//			cellular_rev_end_f = 1;
//			//IDLE_FLAG = 0;
//		}
//		
		if(BG77.rev_start == 1)  //BG77驱动 需要的
		{
		  memcpy(BG77.RxBuf,RxBuf,RxCount);
			BG77.RxCnt = RxCount;
			BG77.rev_end = 1;		
			//IDLE_FLAG = 0;
		}
		
		memset(RxBuf,0,RxCount);
		RxCount = 0;
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetIntStatus(TIM2, TIM_INT_UPDATE) != RESET)
   {
     TIM_ClrIntPendingBit(TIM2, TIM_INT_UPDATE);
		 
		 if(Timer_ms++ == 65530) Timer_ms = 0;
		 if(Timer_ms == 20)
		 {
		   Timer_ms = 0;
//			 Led1Blink();
//			 Led2Blink();
		 }
	 }
}

void DEBUG_USART_IRQHandler(void)
{
	u8 temp;
	if (USART_GetIntStatus(DEBUG_USARTx, USART_INT_RXDNE) != RESET)
    {
        /* Read one byte from the receive data register */
				temp = USART_ReceiveData(DEBUG_USARTx);
				USART_ClrIntPendingBit(DEBUG_USARTx,USART_INT_RXDNE);
    }
	

}

// PA0  wakeup
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE0) != RESET)
    {
			EXTI_ClrITPendBit(EXTI_LINE0);
			
    }
}



void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE10) != RESET)
    {
				EXTI_ClrITPendBit(EXTI_LINE10);
			  SystemInit();
			  USART_DeInit(USART1);
				USART1_INIT();
    }
}



void RTC_WKUP_IRQHandler(void)
{
    EXTI_ClrITPendBit(EXTI_LINE20);
    if (RTC_GetITStatus(RTC_INT_WUT) != RESET)
    {
      RTC_ClrIntPendingBit(RTC_INT_WUT);
    }
}











/**
 * @}
 */
