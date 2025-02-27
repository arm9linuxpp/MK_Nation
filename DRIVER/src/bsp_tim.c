#include "bsp_tim.h"

u16 Timer_ms;
u8  Timer_flag;


static void RCC_Configuration(void)
{
    /* PCLK1 = HCLK/4 */
    RCC_ConfigPclk1(RCC_HCLK_DIV16);

    /* TIM2 clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
}
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

//系统时钟为 144MhZ时  1ms进一次中断 
void Timer_init(void)
{
	TIM_TimeBaseInitType TIM_TimeBaseStructure;
	RCC_Configuration();
	TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.Period    = 65535;   //50ms
	TIM_TimeBaseStructure.Prescaler = 0;
	TIM_TimeBaseStructure.ClkDiv    = 0;
	TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

	TIM_InitTimeBase(TIM2, &TIM_TimeBaseStructure);
	TIM_ConfigInt(TIM2, TIM_INT_UPDATE, ENABLE);
  NVIC_Configuration();
  TIM_Enable(TIM2, ENABLE);
	Timer_ms = 0;
}




