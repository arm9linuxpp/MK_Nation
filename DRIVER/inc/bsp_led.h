#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "main.h"


//#define LED1_PORT   	GPIOB
//#define LED1_PIN   		GPIO_PIN_5

//#define LED2_PORT   	GPIOA
//#define LED2_PIN   		GPIO_PIN_8

#define LED1_PORT   	GPIOB
#define LED1_PIN   		GPIO_PIN_0

#define LED2_PORT   	GPIOA
#define LED2_PIN   		GPIO_PIN_7


void LED1_INIT(void);
void LED1_CMD(u8 cmd);
void LED2_INIT(void);
void LED2_CMD(u8 cmd);
void Led1Blink(void);
void Led2Blink(void);
void LedBlink(GPIO_Module* GPIOx, uint16_t Pin);





















#endif



