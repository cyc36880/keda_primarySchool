#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cw32l031_hal.h"

#define GPIO_POWER_EN_Port CW_GPIOA
#define GPIO_POWER_EN_Pin  GPIO_PIN_9

#define GPIO_IRM_Port CW_GPIOB
#define GPIO_IRM_Pin  GPIO_PIN_3

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
