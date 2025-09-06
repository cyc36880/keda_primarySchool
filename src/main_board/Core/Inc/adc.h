#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct 
{
    void * Instance;    
} ADC_HandleTypeDef;

void MAX_ADC_Init(void);
uint32_t adc_get_value(uint32_t adc_channel);


#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
