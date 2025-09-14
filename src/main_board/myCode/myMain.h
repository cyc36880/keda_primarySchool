#ifndef __MYMAIN_H__
#define __MYMAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "component/udcheck/udc.h"
#include "component/zs_tools/src/zst_core.h"

#define LOCK   1
#define UNLOCK 0

#define DEVICE_ADDR_SPACE(I2CDEV, space)      I2CDEV ## _SPACE = I2CDEV+space-1

enum DEVICE_ADDR
{
    DEVICE_ADDR_SYSTEM=0, // 系统
    DEVICE_ADDR_IRM,      // 红外遥控
    DEVICE_ADDR_BUZZER,   // 蜂鸣器
    DEVICE_ADDR_MISC,     // misc
    DEVICE_ADDR_GRAY,     // 灰度
    DEVICE_ADDR_MOTOR,    // 电机
    DEVICE_ADDR_ULTR,     // 超声波
};

typedef struct 
{
    ptask_t * ptask_root_1;
} ptask_root_1_collection_t;

typedef struct 
{
    ptask_t * ptask_protocol; // 协议
    ptask_t * ptask_sys;      // 系统
    ptask_t * ptask_iremote;  // 红外
    ptask_t * ptask_buzzer;   // 蜂鸣器
    ptask_t * ptask_misc;     // misc
    ptask_t * ptask_gray;     // 灰度
    ptask_t * ptask_motor;    // 电机
    ptask_t * ptask_ultr;     // 超声波
} ptask_1_collection_t;

void setup(void);
void loop(void);


extern ptask_root_1_collection_t ptask_root_1_collection;
extern ptask_1_collection_t ptask_1_collection;

#ifdef __cplusplus
}
#endif

#endif /* __MYMAIN_H__ */
