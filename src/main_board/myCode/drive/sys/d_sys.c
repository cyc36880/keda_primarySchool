/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 13:55:25
 * @LastEditTime : 2025-09-16 20:06:20
 * @Description  :
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_sys.h"
#include "drive/protocol/d_protocol.h"
#include "misc/myMath.h"
#include "adc.h"
#include "gpio.h"
#include "stdlib.h"

#include "drive/iremote/d_iremote.h"
#include "drive/buzzer/d_buzzer.h"
#include "drive/misc/d_misc.h"
#include "drive/motor/d_motor.h"
#include "drive/gray/d_gray.h"
#include "drive/ultr/d_ultr.h"


/******************
 * data struct
 *****************/
#define LOG_TAG "sys"
#define FILTERING_NUM 9 // 滤波数组长度

typedef struct
{
    uint8_t power;     // 电量
    uint8_t sys_start; // 开机
    uint8_t stop_all_device; // 停止所有设备
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t *ptask);
static void timer_cb(zst_timer_t *timer);
static int compare_uint16(const void *a, const void *b);

/********************
 * static variables
 *******************/
static uint16_t adc_value_array[FILTERING_NUM] = {0};
static uint32_t last_tick = 0; // 关机计时
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = "power",
        .data = &dev.power,
        .size = sizeof(dev.power),
        .subscribe = 1, // 默认订阅
    },
    [1] = {
        .name = "sys_start", 
        .data = &dev.sys_start, 
        .size = sizeof(dev.sys_start),
        .subscribe = 1, // 默认订阅
    },
    [2] = {
        .name = "stop_all_device",
        .data = &dev.stop_all_device,
        .size = sizeof(dev.stop_all_device),
    }
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
    .addr = DEVICE_ADDR_SYSTEM,
    .elements_array = element_array,
    .elements_count = sizeof(element_array) / sizeof(element_array[0]),
    .comparison_buffer = comparison_buffer,
};

/********************
 * global variables
 *******************/

/********************
 * global functions
 *******************/
void d_sys_init(void)
{
    /****************
     * 数据包初始化
     ****************/
    dev.sys_start = 1;
    data_pack_add_group(&protocol_data_pack_KX0, &group);
    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback};
    ptask_1_collection.ptask_sys = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_sys)
        ZST_LOGE(LOG_TAG, "ptask_sys create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask_sys create success!");

    /******************
     *   电量定时器
     ****************/
    zst_timer_create(&zst_ztimer, timer_cb, 10, NULL);
}

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t *ptask)
{
    data_element_t * element = NULL;
     DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element,
        if (0 == strcmp("stop_all_device", element->name))
        {
            if (1 == dev.stop_all_device)
                dev.stop_all_device = 0;
            d_iremote_reset();
            d_buzzer_reset();
            d_misc_reset();
            d_motor_reset();
            d_gray_reset();
            d_ultr_reset();
        }
    );

    if (0 == dev.sys_start)
    {
        ZST_LOG("POWER OFF");
        while (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
            ;
        NVIC_SystemReset(); // 复位
    }

    // 关机检测
    if (0 == last_tick && GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 1)
        last_tick = zst_tick_get();
    if (0 != last_tick)
    {
        if (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
        {
            if (zst_tick_elaps(last_tick) > POWER_OFF_PRESS_MS)
            {
                dev.sys_start = 0;
                GPIO_WritePin(GPIO_POWER_EN_Port, GPIO_POWER_EN_Pin, (GPIO_PinState)!POWER_EN_LEVEL);
            }
        }
        else
        {
            last_tick = zst_tick_get();
        }
    }
}

static void timer_cb(zst_timer_t *timer)
{
    // 1.8  ~ 2.0
    // 2233 ~ 2481
    static uint8_t adc_check_count = 0;
    if (adc_check_count < FILTERING_NUM)
    {
        uint16_t adc_value = adc_get_value(ADC_POWER_CHANNEL);
        adc_value_array[adc_check_count++] = min(2481, max(2233, adc_value));
    }
    else
    {
        adc_check_count = 0;
        qsort(adc_value_array, FILTERING_NUM, sizeof(uint16_t), compare_uint16);
        dev.power = number_map(adc_value_array[FILTERING_NUM / 2], 2233, 2481 + 1, 0, 2 + 1);
    }
}

static int compare_uint16(const void *a, const void *b)
{
    const uint16_t *aa = a;
    const uint16_t *bb = b;
    return (*aa > *bb) - (*aa < *bb);
}
