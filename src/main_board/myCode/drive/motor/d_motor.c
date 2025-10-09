/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 17:32:31
 * @LastEditTime : 2025-10-09 16:35:01
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

enum DEV_ELEMENT_NAME
{
    DEV_ELEMENT_NAME_speed=1,
    DEV_ELEMENT_NAME_speedL,
    DEV_ELEMENT_NAME_speedR,
};

typedef struct
{
    int8_t speed[2]; // 轮子转速
} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);
static void ptask_user_message_callback(const ptask_user_message_t *msg);
static void motorL_set_speed(int16_t speed);
static void motorR_set_speed(int16_t speed);

/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_speed),
        .data = &dev.speed,
        .size = sizeof(dev.speed),
    },
    [1] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_speedL),
        .data = &dev.speed[0],
        .size = 1,
    },
    [2] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_speedR),
        .data = &dev.speed[1],
        .size = 1,
    },
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
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
    /****************
     *  设备初始化
     ****************/
    motorL_set_speed(0);
    motorR_set_speed(0);


    /****************
     * 数据包初始化
     ****************/
    data_pack_add_group(&protocol_data_pack_KX0, &group);


    /****************
     * 任务初始化
     ****************/
    ptask_1_collection.ptask_motor = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_motor)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}

/****************************
 * static function
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e)
{
    switch (ptask_get_code(e))
    {
        case PTASK_EVENT_RUN:
            ptask_run_callback(task);
            break;
        
        case PTASK_EVENT_USER:
            ptask_user_message_callback((ptask_user_message_t *)ptask_get_param(e));
            break;
            
        default:
            break;
    }
}

static void ptask_run_callback(ptask_t * ptask)
{
    static data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element, 
        switch ((uintptr_t)element->name)
        {
            case DEV_ELEMENT_NAME_speed:
                ZST_LOG("speed: %d speed: %d", dev.speed[0], dev.speed[1]);
                motorL_set_speed(dev.speed[0]);
                motorR_set_speed(dev.speed[1]);
                break;
            case DEV_ELEMENT_NAME_speedL:
                ZST_LOG("speedL: %d", dev.speed[0]);
                motorL_set_speed(dev.speed[0]);
                break;
            case DEV_ELEMENT_NAME_speedR:
                ZST_LOG("speedR: %d", dev.speed[1]);
                motorR_set_speed(dev.speed[1]);
                break;
            default:
                break;
        }
    );
}


static void ptask_user_message_callback(const ptask_user_message_t *msg)
{
    switch (msg->message_type)
    {
        case PTASK_MESSAGE_TYPE_RESET_DEVICE:
            dev.speed[0] = 0;
            dev.speed[1] = 0;
            element_array[0].receive_change_flag= 1;
            break;

        default:
            break;
    }
}


static void motorR_set_speed(int16_t speed)
{
    speed = min(100, max(-100, speed));
    uint16_t speed_abs = myabs(speed);
    
    if (speed_abs == 0)
    {
        ATIM_SetCompare1A(0);
        ATIM_SetCompare2A(0);
        return;
    }

    speed_abs = number_map(speed_abs, 0, 100+1, 400, 1000+1);

    if (speed > 0)
    {
        ATIM_SetCompare1A(0);
        ATIM_SetCompare2A(speed_abs);
    }
    else
    {
        ATIM_SetCompare1A(speed_abs);
        ATIM_SetCompare2A(0);
    }
}

static void motorL_set_speed(int16_t speed)
{
    speed = min(100, max(-100, speed));
    uint16_t speed_abs = myabs(speed);
    if (speed_abs == 0)
    {
        ATIM_SetCompare3B(0);
        ATIM_SetCompare2B(0);
        return;
    }

    speed_abs = number_map(speed_abs, 0, 100+1, 400, 1000+1);

    if (speed > 0)
    {
        ATIM_SetCompare3B(0);
        ATIM_SetCompare2B(speed_abs);
    }
    else
    {
        ATIM_SetCompare3B(speed_abs);
        ATIM_SetCompare2B(0);
    }
}

