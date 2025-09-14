/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 13:55:25
 * @LastEditTime : 2025-09-12 19:33:01
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_sys.h"
#include "drive/protocol/d_protocol.h"


/******************
 * data struct 
 *****************/
#define LOG_TAG "sys"

typedef struct
{
    uint8_t power;    // 电量

} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = "power",
        .data = &dev.power,
        .size = sizeof(dev.power),
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
    
}

