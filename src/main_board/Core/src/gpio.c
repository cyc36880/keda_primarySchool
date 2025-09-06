#include "gpio.h"

void MAX_GPIO_Init(void)
{
    __RCC_GPIOA_CLK_ENABLE();
    __RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pins = GPIO_POWER_EN_Pin;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
    GPIO_WritePin(CW_GPIOA, GPIO_POWER_EN_Pin, GPIO_Pin_SET);

    GPIO_InitStructure.IT = GPIO_IT_RISING | GPIO_IT_FALLING;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pins = GPIO_IRM_Pin;
    GPIO_Init(GPIO_IRM_Port, &GPIO_InitStructure);

    GPIO_ConfigFilter(GPIO_IRM_Port, bv1, GPIO_FLTCLK_RC150K);
    GPIOA_INTFLAG_CLR(bv1| bv2);
    NVIC_EnableIRQ(GPIOB_IRQn);
}

/***************************
 * 中断
 **************************/
static void GPIO_ExitCallback(uint16_t pin);
void UART_Callback(uint16_t pin)
{

}

static void GPIO_ExitCallback(uint16_t pin)
{

}


