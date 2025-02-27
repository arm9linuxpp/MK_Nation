#include "bsp_adc.h"
#include <stdio.h>
ADC_InitType ADC_InitStructure;
DMA_InitType DMA_InitStructure;
uint16_t ADCConvertedValue;

/**
 * @brief  Configures the different system clocks.
 */
static void RCC_Configuration(void)
{
    /* Enable peripheral clocks ------------------------------------------------*/

    /* Enable GPIOC clocks */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    /* Enable ADC1, ADC2, ADC3 and ADC4 clocks */
    RCC_EnableAHBPeriphClk( RCC_AHB_PERIPH_ADC2 ,ENABLE);

    /* RCC_ADCHCLK_DIV16*/
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB,RCC_ADCHCLK_DIV16);
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8);  //selsect HSE as RCC ADC1M CLK Source		
}

/**
 * @brief  Configures the different GPIO ports.
 */
static void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* Configure PC0 PC1 as analog input -------------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

}

void ADC_GetData(void)
{ 
    
    ADC_ConfigRegularChannel(ADC2, ADC2_Channel_02_PA5, 1, ADC_SAMP_TIME_239CYCLES5);
    /* Start ADC Software Conversion */
    ADC_EnableSoftwareStartConv(ADC2, ENABLE);
    while(ADC_GetFlagStatus(ADC2, ADC_FLAG_ENDC)==0){
    }
    ADC_ClearFlag(ADC2, ADC_FLAG_ENDC);
    ADC_ClearFlag(ADC2, ADC_FLAG_STR);
    ADCConvertedValue = ADC_GetDat(ADC2);
		
		printf("ADC:%d\n",ADCConvertedValue);
}


void ADC_Initial(ADC_Module* ADCx)
{
	  RCC_Configuration();
	  GPIO_Configuration();
    /* ADC configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_Init(ADCx, &ADC_InitStructure);


    /* Enable ADC */
    ADC_Enable(ADCx, ENABLE);
    /*Check ADC Ready*/
    while(ADC_GetFlagStatusNew(ADCx,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC calibration */
    ADC_StartCalibration(ADCx);
    /* Check the end of ADC calibration */
    while (ADC_GetCalibrationStatus(ADCx))
        ;
}


void  ADC2_Init(void)
{
  ADC_Initial(ADC2);
}







