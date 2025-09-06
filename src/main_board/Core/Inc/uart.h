#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct 
{
    void* Instance;
} UART_HandleTypeDef;

void MAX_UART1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
