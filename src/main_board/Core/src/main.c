#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "uart.h"
#include "adc.h"

#include "myMain.h"

static void RCC_Configuration(void);

int main()
{
    RCC_Configuration();
    MAX_GPIO_Init();
    MAX_ATIM_Init();
    MAX_GTIM2_Init();
    MAX_BTIM1_Init();
    MAX_UART1_Init();
    MAX_ADC_Init();
    
    setup();
    while(1)
    {
        loop();
    }
}


/**
 * @brief  Configures the different system clocks.
 * @param  None
 * @retval None
 */
static void RCC_Configuration(void)
{
    __RCC_FLASH_CLK_ENABLE();
    FLASH_SetLatency(FLASH_Latency_2);

    /* 0. HSI使能并校准 */
    RCC_HSI_Enable(RCC_HSIOSC_DIV1);

    /* 1. 设置HCLK和PCLK的分频系数　*/
    RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
    RCC_PCLKPRS_Config(RCC_PCLK_DIV1);

    RCC_SystemCoreClockUpdate(48000000);
    InitTick(48000000);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
