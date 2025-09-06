#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct 
{
    void* Instance;
} TIME_HandleTypeDef;

void MAX_ATIM_Init(void);
void MAX_GTIM2_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
