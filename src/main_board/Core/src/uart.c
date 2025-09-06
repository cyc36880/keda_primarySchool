#include "uart.h"
#include <stdio.h>

static void UART_GPIO_Init(UART_HandleTypeDef *uartHandle);

UART_HandleTypeDef uart1;

void MAX_UART1_Init(void)
{
    UART_GPIO_Init(&uart1);

    __RCC_UART1_CLK_ENABLE();

    USART_InitTypeDef USART_InitStructure = {0};

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_Over = USART_Over_16;
    USART_InitStructure.USART_Source = USART_Source_PCLK;
    USART_InitStructure.USART_UclkFreq = 48000000;
    USART_InitStructure.USART_StartBit = USART_StartBit_FE;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(CW_UART1, &USART_InitStructure);

    USART_ITConfig(CW_UART1, USART_IT_RC, ENABLE);
}

static void UART_GPIO_Init(UART_HandleTypeDef *uartHandle)
{
    __RCC_GPIOB_CLK_ENABLE();

    //UART TX RX 复用
    PA08_AFx_UART1TXD();
    PA09_AFx_UART1RXD();
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.Pins = GPIO_PIN_8;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pins = GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);    
}

/*****************************
 * 重定向c库函数printf到USARTx
 *****************************/
int fputc(int ch, FILE *f)
{
    USART_SendData_8bit(CW_UART1, (uint8_t)ch);
    while (USART_GetFlagStatus(CW_UART1, USART_FLAG_TXE) == RESET);
    return ch;
}

/***************************
 * UART接收完成回调函数
 ***************************/
static void UART_RxCpltCallback(UART_HandleTypeDef *uart);
void UART_Callback(UART_TypeDef *UARTx)
{
    if(USART_GetITStatus(UARTx, USART_IT_RC) != RESET)
    {
        UART_RxCpltCallback(&uart1);
        USART_ClearITPendingBit(UARTx, USART_IT_RC);   
    }
}

static void UART_RxCpltCallback(UART_HandleTypeDef *uart)
{
    
}
