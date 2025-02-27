#ifndef __BSP_RCC_H_
#define __BSP_RCC_H_
#include "n32g45x.h"
#include "debug.h"



enum
{
    SYSCLK_PLLSRC_HSI,
    SYSCLK_PLLSRC_HSE,
};


	





extern RCC_ClocksType RCC_ClockFreq;
void DumpClock(const char* msg);
void SetSysClockToHSI(void);
void SetSysClockToHSE(void);
void SetSysClockToPLL(uint32_t freq, uint8_t src);
#endif















