#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cw32l031_hal.h"

#define POWER_EN_LEVEL   GPIO_Pin_SET

// 电源开关
#define GPIO_POWER_Port CW_GPIOF
#define GPIO_POWER_Pin  GPIO_PIN_6

// 电源控制
#define GPIO_POWER_EN_Port CW_GPIOF
#define GPIO_POWER_EN_Pin  GPIO_PIN_7

// 红外遥控器
#define GPIO_IRM_Port CW_GPIOB
#define GPIO_IRM_Pin  GPIO_PIN_3

// 灰度RGB
#define GPIO_GRAY_B_Port CW_GPIOB
#define GPIO_GRAY_B_Pin  GPIO_PIN_0
#define GPIO_GRAY_G_Port CW_GPIOB
#define GPIO_GRAY_G_Pin  GPIO_PIN_1
#define GPIO_GRAY_R_Port CW_GPIOB
#define GPIO_GRAY_R_Pin  GPIO_PIN_2

// ADC
#define ADC_POWER_CHANNEL             ADC_ExInputCH0  // 电量
#define ADC_LIGHT_CHANNEL             ADC_ExInputCH1  // 光线
#define ADC_RHEOSTAT_CHANNEL          ADC_ExInputCH2  // 电位器
#define ADC_GRAY_0_CHANNEL            ADC_ExInputCH7  // 灰度0
#define ADC_GRAY_1_CHANNEL            ADC_ExInputCH6  // 灰度1
#define ADC_GRAY_2_CHANNEL            ADC_ExInputCH5  // 灰度2
#define ADC_GRAY_3_CHANNEL            ADC_ExInputCH4  // 灰度3
#define ADC_GRAY_4_CHANNEL            ADC_ExInputCH3  // 灰度4
#define ADC_INFRARED_SENSOR_CHANNEL   ADC_ExInputCH11 // 红外对管

// 按键
#define KEY0_Port CW_GPIOB
#define KEY0_Pin  GPIO_PIN_4
#define KEY1_Port CW_GPIOB
#define KEY1_Pin  GPIO_PIN_5
#define KEY2_Port CW_GPIOB
#define KEY2_Pin  GPIO_PIN_6
#define KEY3_Port CW_GPIOB
#define KEY3_Pin  GPIO_PIN_7

// 红外对管
#define INFRARED_Port CW_GPIOB 
#define INFRARED_Pin  GPIO_PIN_10

// 超声波
#define ULTR_TRIG_Port CW_GPIOA
#define ULTR_TRIG_Pin  GPIO_PIN_11
#define ULTR_ECHO_Port CW_GPIOA
#define ULTR_ECHO_Pin  GPIO_PIN_10

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
