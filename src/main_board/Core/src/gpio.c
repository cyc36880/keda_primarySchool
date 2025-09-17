#include "gpio.h"
#include "drive/iremote/d_iremote.h"
#include "drive/ultr/d_ultr.h"

void MAX_GPIO_Init(void)
{
    __RCC_GPIOA_CLK_ENABLE();
    __RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    /**********************
     *      key
     *********************/
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStructure.Pins = KEY0_Pin | KEY1_Pin | KEY2_Pin | KEY3_Pin;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);


    /**********************
     *    红外对管
     *********************/
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStructure.Pins = INFRARED_Pin;
    GPIO_Init(INFRARED_Port, &GPIO_InitStructure);
    

    /*************************
     *      超声波
     ************************/
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pins = ULTR_TRIG_Pin;
    GPIO_Init(ULTR_TRIG_Port, &GPIO_InitStructure);

    GPIO_InitStructure.IT = GPIO_IT_RISING | GPIO_IT_FALLING;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStructure.Pins = ULTR_ECHO_Pin;
    GPIO_Init(ULTR_ECHO_Port, &GPIO_InitStructure);
    NVIC_EnableIRQ(GPIOA_IRQn);

    /**********************
     *       开关
     *********************/
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStructure.Pins = GPIO_POWER_Pin;
    GPIO_Init(GPIO_POWER_Port, &GPIO_InitStructure);


    /**********************
     *       电源
     *********************/
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pins = GPIO_POWER_EN_Pin;
    GPIO_Init(GPIO_POWER_EN_Port, &GPIO_InitStructure);
    GPIO_WritePin(GPIO_POWER_EN_Port, GPIO_POWER_EN_Pin, (GPIO_PinState)!POWER_EN_LEVEL);

    
    /**********************
     *     红外遥控器
     *********************/
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
        iremote_gpio_interrupt_callback(&iremote); // 红外
    }
    else if (gpio == ULTR_ECHO_Port && pin == ULTR_ECHO_Pin) //超声波
    { 
        ultr_gpio_interrupt_callback(&ultr);
    }
}


