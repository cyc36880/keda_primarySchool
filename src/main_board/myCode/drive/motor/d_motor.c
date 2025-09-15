/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 17:32:31
 * @LastEditTime : 2025-09-15 11:22:22
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_motor.h"
#include "drive/protocol/d_protocol.h"
#include "tim.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "motor"

typedef struct
{
    int8_t speed[2]; // 左右轮子转速
} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static void motorL_set_speed(int16_t speed);
static void motorR_set_speed(int16_t speed);

/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = "speed",
        .data = &dev.speed,
        .size = sizeof(dev.speed),
    },
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = "motor",
    .addr = DEVICE_ADDR_MOTOR,
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
void d_motor_init(void)
{
    motorL_set_speed(80);
    motorR_set_speed(80);


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
    ptask_1_collection.ptask_motor = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_motor)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    static data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element, 
        if (0 == strcmp(element->name, "speed"))
        {
            ZST_LOG("speed%d speed: %d", dev.speed[0], dev.speed[1]);
            motorL_set_speed(dev.speed[0]);
            motorR_set_speed(dev.speed[1]);
        }
    );
}

static void motorL_set_speed(int16_t speed)
{
    speed *= 10;
    speed = speed > 1000 ? 1000 : speed;
    speed = speed < -1000 ? -1000 : speed;
    if (speed > 0)
    {
        ATIM_SetCompare2A(speed);
        ATIM_SetCompare3A(0);
    }
    else
    {
        ATIM_SetCompare2A(0);
        ATIM_SetCompare3A(-speed);
    }
}

static void motorR_set_speed(int16_t speed)
{
    speed *= 10;
    speed = speed > 1000 ? 1000 : speed;
    speed = speed < -1000 ? -1000 : speed;
    if (speed > 0)
    {
        GTIM_SetCompare2(CW_GTIM2, speed);
        GTIM_SetCompare1(CW_GTIM2, 0);
    }
    else
    {
        GTIM_SetCompare2(CW_GTIM2, 0);
        GTIM_SetCompare1(CW_GTIM2, -speed);
    }
}

