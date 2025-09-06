#include "adc.h"

static void ADC_GPIO_Init(ADC_HandleTypeDef *adcHandle);
static ADC_SingleChTypeDef ADC_SingleChStructure = {0};

ADC_HandleTypeDef adc1;

void MAX_ADC_Init(void)
{
    ADC_GPIO_Init(&adc1);

    __RCC_ADC_CLK_ENABLE();

    ADC_InitTypeDef ADC_InitStructure = {0};
    ADC_WdtTypeDef ADC_WdtStructure = {0};

    adc1.Instance = CW_ADC;
    //ADC默认值初始化
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div1;
    //ADC模拟看门狗通道初始化
    ADC_WdtInit(&ADC_WdtStructure);

    // ADC_ITConfig(ADC_IT_EOC | ADC_IT_EOA, ENABLE);
    // ADC_EnableIrq(ADC_INT_PRIORITY);
    // ADC_ClearITPendingAll();

    // 使能
    ADC_Enable();

    //配置单通道转换模式
    ADC_SingleChStructure.ADC_DiscardEn = ADC_DiscardNull;
    ADC_SingleChStructure.ADC_InitStruct = ADC_InitStructure;
    ADC_SingleChStructure.ADC_WdtStruct = ADC_WdtStructure;
}

/**
 * @param
 *  ADC_ExInputCH0
 *  ADC_ExInputCH1
 *  ADC_ExInputCH2
 *  ADC_ExInputCH3
 *  ADC_ExInputCH4
 *  ADC_ExInputCH5
 *  ADC_ExInputCH6
 *  ADC_ExInputCH7
 *  ADC_ExInputCH8
 *  ADC_ExInputCH9
 *  ADC_ExInputCH10
 *  ADC_ExInputCH11
 *  ADC_ExInputCH12
 */
uint32_t adc_get_value(uint32_t adc_channel)
{
    ADC_SingleChStructure.ADC_Chmux = adc_channel;
    ADC_SingleChOneModeCfg(&ADC_SingleChStructure);
    ADC_SoftwareStartConvCmd(ENABLE);
    while (0 == CW_ADC->ISR_f.EOC);
    return ADC_GetConversionValue();
}



static void ADC_GPIO_Init(ADC_HandleTypeDef *adcHandle)
{
    __RCC_GPIOA_CLK_ENABLE();

    PA00_ANALOG_ENABLE();
    PA01_ANALOG_ENABLE();
    PA02_ANALOG_ENABLE();
    PA03_ANALOG_ENABLE();
    PA04_ANALOG_ENABLE();
    PA05_ANALOG_ENABLE();
    PA06_ANALOG_ENABLE();
    PA07_ANALOG_ENABLE();
}


/*****************
 * 中断
 ****************/
static void ADC_ConversionCompleteCallback(ADC_HandleTypeDef *adc);
void ADC_Callback(ADC_TypeDef *adc)
{
//    static uint8_t gFlagIrq;
//    gFlagIrq = CW_ADC->ISR;
	CW_ADC->ICR = 0x00;
    ADC_ConversionCompleteCallback(&adc1);
}

static void ADC_ConversionCompleteCallback(ADC_HandleTypeDef *adc)
{

}

