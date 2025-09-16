/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 17:32:31
 * @LastEditTime : 2025-09-16 18:53:33
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_motor.h"
#include "drive/protocol/d_protocol.h"
#include "tim.h"
#include "misc/myMath.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "motor"

typedef struct
{
    int8_t speed[2]; // 轮子转速
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
    [1] = {
        .name = "speedL",
        .data = &dev.speed[0],
        .size = 1,
    },
    [2] = {
        .name = "speedR",
        .data = &dev.speed[1],
        .size = 1,
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
    motorL_set_speed(0);
    motorR_set_speed(0);


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

void d_motor_reset(void)
{
    dev.speed[0] = 0;
    dev.speed[1] = 0;
    element_array[0].receive_change_flag = 1;
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
            ZST_LOG("speed: %d speed: %d", dev.speed[0], dev.speed[1]);
            motorL_set_speed(dev.speed[0]);
            motorR_set_speed(dev.speed[1]);
        }
        else if (0 == strcmp(element->name, "speedL"))
        {
            ZST_LOG("speedL: %d", dev.speed[0]);
            motorL_set_speed(dev.speed[0]);
        }
        else if (0 == strcmp(element->name, "speedR"))
        {
            ZST_LOG("speedR: %d", dev.speed[1]);
            motorR_set_speed(dev.speed[1]);
        }
    );
}

static void motorL_set_speed(int16_t speed)
{
    speed = min(100, max(-100, speed));
    uint16_t speed_abs = myabs(speed);
    
    if (speed_abs == 0)
    {
        ATIM_SetCompare2A(0);
        ATIM_SetCompare3A(0);
        return;
    }

    speed_abs = number_map(speed_abs, 0, 100+1, 400, 1000+1);

    if (speed > 0)
    {
        ATIM_SetCompare2A(speed_abs);
        ATIM_SetCompare3A(0);
    }
    else
    {
        ATIM_SetCompare2A(0);
        ATIM_SetCompare3A(speed_abs);
    }
}

static void motorR_set_speed(int16_t speed)
{
    speed = min(100, max(-100, speed));
    uint16_t speed_abs = myabs(speed);
    if (speed_abs == 0)
    {
        GTIM_SetCompare2(CW_GTIM2, 0);
        GTIM_SetCompare1(CW_GTIM2, 0);
        return;
    }

    speed_abs = number_map(speed_abs, 0, 100+1, 400, 1000+1);

    if (speed > 0)
    {
        GTIM_SetCompare2(CW_GTIM2, speed_abs);
        GTIM_SetCompare1(CW_GTIM2, 0);
    }
    else
    {
        GTIM_SetCompare2(CW_GTIM2, 0);
        GTIM_SetCompare1(CW_GTIM2, speed_abs);
    }
}

