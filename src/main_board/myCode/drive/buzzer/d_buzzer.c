/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 20:54:06
 * @LastEditTime : 2025-09-16 18:27:24
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_buzzer.h"
#include "drive/protocol/d_protocol.h"
#include "tim.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG  "buzzer"
typedef struct
{
    uint8_t status;
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static void set_buzzer_status(uint8_t status);

/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = "status",
        .data = &dev.status,
        .size = sizeof(dev.status),
    },
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
    .addr = DEVICE_ADDR_BUZZER,
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
void d_buzzer_init(void)
{
    /****************
     * 数据包初始化
     ****************/
    data_pack_add_group(&protocol_data_pack_KX0, &group);
    set_buzzer_status(0);

    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_buzzer = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_buzzer)
        ZST_LOGE(LOG_TAG, "ptask_buzzer create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask_buzzer create success!");
}

void d_buzzer_reset(void)
{
    dev.status = 0;
    element_array[0].receive_change_flag = 1;
}

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element,
        if (0 == strcmp("status", element->name))
        {
            set_buzzer_status(dev.status);
        }
    );
}

static void set_buzzer_status(uint8_t status)
{
    ATIM_SetCompare1B((bool)status * 500);
}


