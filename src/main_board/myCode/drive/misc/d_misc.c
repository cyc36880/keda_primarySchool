/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 20:59:33
 * @LastEditTime : 2025-09-15 10:39:54
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
/************************************************
 * 此文件包含功能杂项：
 * 1. 光线
 * 2. 电阻器
 * 3. 红外对管
 * 4. 按键*4
 ***********************************************/
#include "d_misc.h"
#include "gpio.h"
#include "drive/protocol/d_protocol.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "misc"

typedef struct
{
    uint8_t light;    // 光线
    uint8_t res;      // 电阻器
    uint8_t infrared; // 红外对管
    uint8_t key[4];   // 按键*4
} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static void set_led_color(uint8_t r, uint8_t g, uint8_t b);

/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = "light",
        .data = &dev.light,
        .size = sizeof(dev.light),
    },
    [1] = {
        .name = "res",
        .data = &dev.res,
        .size = sizeof(dev.res),
    },
    [2] = {
        .name = "infrared",
        .data = &dev.infrared,
        .size = sizeof(dev.infrared),
    },
    [3] = {
        .name = "key",
        .data = &dev.key,
        .size = sizeof(dev.key),
    }
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = "misc",
    .addr = DEVICE_ADDR_MISC,
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
void d_misc_init(void)
{
    set_led_color(0, 0, 0);
    /****************
     * 数据包初始化
     ****************/
    data_pack_add_group(&protocol_data_pack_KX0, &group);

    
    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_misc = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_misc)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    dev.key[0] = GPIO_ReadPin(KEY0_Port, KEY0_Pin);
    dev.key[1] = GPIO_ReadPin(KEY1_Port, KEY1_Pin);
    dev.key[2] = GPIO_ReadPin(KEY2_Port, KEY2_Pin);
    dev.key[3] = GPIO_ReadPin(KEY3_Port, KEY3_Pin);
}


static void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    ATIM_SetCompare2B(1000 - r*1000/255);
    ATIM_SetCompare3B(1000 - g*1000/255);
    ATIM_SetCompare1A(1000 - b*1000/255);
}
