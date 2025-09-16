/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 13:55:25
 * @LastEditTime : 2025-09-16 11:53:39
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_sys.h"
#include "drive/protocol/d_protocol.h"
#include "misc/myMath.h"
#include "adc.h"
#include "gpio.h"
#include "cmsis_armcc.h"
/******************
 * data struct 
 *****************/
#define LOG_TAG "sys"

typedef struct
{
    uint8_t power;     // 电量
    uint8_t sys_start; // 开机
} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
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
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = "sys",
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
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_sys = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_sys)
        ZST_LOGE(LOG_TAG, "ptask_sys create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask_sys create success!");
}



/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    dev.power = number_map(adc_get_value(ADC_POWER_CHANNEL), 0, 4095+1, 0, 100+1);
//
    if (0 == dev.sys_start)
    {
        ZST_LOG("POWER OFF");
        while (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
            ;
        NVIC_SystemReset(); // 复位
    }

   // 关机检测
   if (0 == last_tick)
       last_tick = zst_tick_get();
   if (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
   {
       if (zst_tick_elaps(last_tick) > POWER_OFF_PRESS_MS)
       {
           dev.sys_start = 0;
           GPIO_WritePin(GPIO_POWER_EN_Port, GPIO_POWER_EN_Pin, !POWER_EN_LEVEL);
       }
   }
   else
   {
       last_tick = zst_tick_get();
   }
}

