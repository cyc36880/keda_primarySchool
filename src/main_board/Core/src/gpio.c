#include "gpio.h"
#include "drive/iremote/d_iremote.h"

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
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStructure.Pins = GPIO_IRM_Pin;
    GPIO_Init(GPIO_IRM_Port, &GPIO_InitStructure);

    // GPIO_ConfigFilter(GPIO_IRM_Port, bv3, GPIO_FLTCLK_RC150K);
    // GPIOB_INTFLAG_CLR(bv3);
    NVIC_EnableIRQ(GPIOB_IRQn);
}

/***************************
 * 中断
 **************************/
static void GPIO_ExitCallback(GPIO_TypeDef * gpio,  uint16_t pin);
void GPIO_Callback(GPIO_TypeDef * gpio)
{
    uint8_t pos_pin = 0;
    uint32_t pin = 0;
    for (uint8_t i=0; i<16; i++)
    {
        if (gpio->ISR & (1<<i))
        {
            pos_pin = i;
            break;
        }
    }
    pin = 1 << pos_pin;
    gpio->ICR = (~((uint32_t)(pin))); // 清除中断标志
    GPIO_ExitCallback(gpio, pin);
}

static void GPIO_ExitCallback(GPIO_TypeDef * gpio,  uint16_t pin)
{
    if (gpio == GPIO_IRM_Port && pin == GPIO_IRM_Pin)
    {
        iremote_gpio_interrupt_callback(&iremote);
    }
}


