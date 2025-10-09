#include "tim.h"
#include "drive/iremote/d_iremote.h"
#include "drive/ultr/d_ultr.h"

static void TIM_GPIO_Init(TIME_HandleTypeDef *timeHandle);

TIME_HandleTypeDef atim = {0};
TIME_HandleTypeDef gtim2 = {0};
TIME_HandleTypeDef btim1 = {0};

void MAX_ATIM_Init(void)
{
    TIM_GPIO_Init(&atim);

    __RCC_ATIM_CLK_ENABLE();

    ATIM_InitTypeDef ATIM_InitStruct = {0};
    ATIM_OCInitTypeDef ATIM_OCInitStruct = {0};
    ATIM_InitStruct.BufferState = DISABLE;
    ATIM_InitStruct.ClockSelect = ATIM_CLOCK_PCLK;
    ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;
    ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;
    ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;
    ATIM_InitStruct.OverFlowMask = DISABLE;
    ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV8; 
    ATIM_InitStruct.ReloadValue = 1000 - 1;                
    ATIM_InitStruct.RepetitionCounter = 0;
    ATIM_InitStruct.UnderFlowMask = DISABLE;

    ATIM_Init(&ATIM_InitStruct);

    ATIM_OCInitStruct.BufferState = DISABLE;
    ATIM_OCInitStruct.OCDMAState = DISABLE;
    ATIM_OCInitStruct.OCInterruptSelect = ATIM_OC_IT_NONE;
    ATIM_OCInitStruct.OCInterruptState = DISABLE;
    ATIM_OCInitStruct.OCMode = ATIM_OCMODE_PWM1;
    ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT;

    ATIM_OC1AInit(&ATIM_OCInitStruct);
    ATIM_OC2AInit(&ATIM_OCInitStruct);
    // ATIM_OC3AInit(&ATIM_OCInitStruct);
    ATIM_OC1BInit(&ATIM_OCInitStruct);
    ATIM_OC2BInit(&ATIM_OCInitStruct);
    ATIM_OC3BInit(&ATIM_OCInitStruct);

    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(0);
    // ATIM_SetCompare3A(0);
    ATIM_SetCompare1B(0);
    ATIM_SetCompare2B(0);
    ATIM_SetCompare3B(0);

    ATIM_CtrlPWMOutputs(ENABLE);
    ATIM_Cmd(ENABLE);
}

void MAX_GTIM2_Init(void)
{
    TIM_GPIO_Init(&gtim2);

    __RCC_GTIM2_CLK_ENABLE();

    GTIM_InitTypeDef GTIM_InitStruct = {0};
    GTIM_InitStruct.Mode = GTIM_MODE_TIME;
    GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;
    GTIM_InitStruct.Prescaler = 8 - 1;    // 计数时钟TCLKD = TCLK/(PSC+1)
    GTIM_InitStruct.ReloadValue = 1000 - 1;
    GTIM_InitStruct.ToggleOutState = DISABLE;

    GTIM_TimeBaseInit(CW_GTIM2, &GTIM_InitStruct);
    GTIM_OCInit(CW_GTIM2, GTIM_CHANNEL2, GTIM_OC_OUTPUT_PWM_LOW);
    GTIM_OCInit(CW_GTIM2, GTIM_CHANNEL1, GTIM_OC_OUTPUT_PWM_LOW);

    GTIM_SetCompare1(CW_GTIM2, 0);
    GTIM_SetCompare2(CW_GTIM2, 0);

    GTIM_Cmd(CW_GTIM2, ENABLE);
}

void MAX_BTIM1_Init(void)
{
    __RCC_BTIM_CLK_ENABLE();

    /*********************
     * 产生us级tick的定时器
     ********************/
    BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct = {0};
    BTIM_TimeBaseInitStruct.BTIM_Mode = BTIM_Mode_TIMER;
    BTIM_TimeBaseInitStruct.BTIM_Prescaler = 32 - 1;   
    BTIM_TimeBaseInitStruct.BTIM_Period = 65535 - 1;

    BTIM_TimeBaseInit(CW_BTIM1, &BTIM_TimeBaseInitStruct);
    BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
    BTIM_Cmd(CW_BTIM1, ENABLE);
    NVIC_EnableIRQ(BTIM1_IRQn);
}


static void TIM_GPIO_Init(TIME_HandleTypeDef *timeHandle)
{
    if (&atim == timeHandle)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        // PB00_AFx_ATIMCH2B();
        // PB01_AFx_ATIMCH3B();
        // PB02_AFx_ATIMCH1A();
        // PB10_AFx_ATIMCH2A();
        // PB11_AFx_ATIMCH3A();
        // PA15_AFx_ATIMCH1B();

        PA08_AFx_ATIMCH1A();
        PA09_AFx_ATIMCH2A();
        PB15_AFx_ATIMCH3B();
        PB14_AFx_ATIMCH2B();
        PA15_AFx_ATIMCH1B();

        // 引脚初始化必须放在复用之后
        GPIO_InitStruct.IT = GPIO_IT_NONE;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_15;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_15 | GPIO_PIN_14;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
    else if (&gtim2 == timeHandle)
    {
        __RCC_GPIOB_CLK_ENABLE();

        PB15_AFx_GTIM2CH2();
        PB15_DIGTAL_ENABLE();
        PB15_DIR_OUTPUT();
        PB15_PUSHPULL_ENABLE();

        PB14_AFx_GTIM2CH1();
        PB14_DIGTAL_ENABLE();
        PB14_DIR_OUTPUT();
        PB14_PUSHPULL_ENABLE();
    }
}

/*******************
 * 中断
 ******************/
void TIM_PeriodElapsedCallback(TIME_HandleTypeDef * tim);
void TIM_Callback(void * tim)
{
    if (CW_BTIM1 == tim)
    {
        TIM_PeriodElapsedCallback(&btim1);
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
    }
    else if (CW_BTIM2 == tim)
    {
        // TIM_PeriodElapsedCallback(&btim2);
        BTIM_ClearITPendingBit(CW_BTIM2, BTIM_IT_OV);
    }
}

void TIM_PeriodElapsedCallback(TIME_HandleTypeDef * tim)
{
    if (&btim1 == tim)
    {
        iremote_timOverFlow_callback(&iremote);
        ultr_timOverFlow_callback(&ultr);
    }
}

