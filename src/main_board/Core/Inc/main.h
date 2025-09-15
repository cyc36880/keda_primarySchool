#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cw32l031_hal.h"

// 电源控制
#define GPIO_POWER_EN_Port CW_GPIOA
#define GPIO_POWER_EN_Pin  GPIO_PIN_9


// 红外遥控器
#define GPIO_IRM_Port CW_GPIOB
#define GPIO_IRM_Pin  GPIO_PIN_13


// ADC
#define ADC_POWER_CHANNEL     ADC_ExInputCH0 // 电量
#define ADC_LIGHT_CHANNEL     ADC_ExInputCH1 // 光线
#define ADC_RHEOSTAT_CHANNEL  ADC_ExInputCH2 // 电位器
#define ADC_GRAY_0_CHANNEL    ADC_ExInputCH3 // 灰度0
#define ADC_GRAY_1_CHANNEL    ADC_ExInputCH3 // 灰度1
#define ADC_GRAY_2_CHANNEL    ADC_ExInputCH3 // 灰度2
#define ADC_GRAY_3_CHANNEL    ADC_ExInputCH3 // 灰度3
#define ADC_GRAY_4_CHANNEL    ADC_ExInputCH3 // 灰度4

#define KEY0_Port CW_GPIOB
#define KEY0_Pin  GPIO_PIN_4
#define KEY1_Port CW_GPIOB
#define KEY1_Pin  GPIO_PIN_5
#define KEY2_Port CW_GPIOB
#define KEY2_Pin  GPIO_PIN_6
#define KEY3_Port CW_GPIOB
#define KEY3_Pin  GPIO_PIN_7

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
