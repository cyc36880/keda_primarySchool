/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 17:32:07
 * @LastEditTime : 2025-10-09 18:22:48
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_ultr.h"
#include "drive/protocol/d_protocol.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "ultr"

enum DEV_ELEMENT_NAME
{
    DEV_ELEMENT_NAME_status=1,
    DEV_ELEMENT_NAME_value,
};

typedef struct
{
    uint8_t status;
    uint8_t value[2];
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void timer_callback(zst_timer_t *timer);
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);
static void ptask_user_message_callback(const ptask_user_message_t *msg);
static uint16_t get_ultr_distance(void);
static uint8_t get_gpio_pin_level(struct _ultr * iremote);
static void set_trig_level(struct _ultr * ultr, uint8_t level);
static uint32_t get_countVal(struct _ultr *iremote);

/********************
 * static variables
 *******************/
static zst_timer_t  * timer_handle = NULL;
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_status),
        .data = &dev.status,
        .size = sizeof(dev.status),
    },
    [1] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_value),
        .data = &dev.value,
        .size = sizeof(dev.value),
    },
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
    .addr = DEVICE_ADDR_ULTR,
    .elements_array = element_array,
    .elements_count = sizeof(element_array) / sizeof(element_array[0]),
    .comparison_buffer = comparison_buffer,
};

/********************
 * global variables
 *******************/
ultr_t ultr = {0};

/********************
 * global functions
 *******************/
void d_ultr_init(void)
{
    /****************
     * 数据包初始化
     ****************/
    data_pack_add_group(&protocol_data_pack_KX0, &group);

    /****************
     * 设备初始化
     ****************/
    ultr.get_echo_pin_level = get_gpio_pin_level,
    ultr.get_countVal = get_countVal,
    ultr.set_trig_pin_level = set_trig_level,
    ultr_init(&ultr, 0.667, 65535);

    /****************
     * 任务初始化
     ****************/
    ptask_1_collection.ptask_ultr =  ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_ultr)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");

    /****************
     * 定时器初始化
     ****************/
    timer_handle = zst_timer_create(&zst_ztimer, timer_callback, 200, NULL);
    zst_timer_pause(timer_handle);
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

static void timer_callback(zst_timer_t *timer)
{
    uint16_t ultrvalue = get_ultr_distance();
    ultrvalue /= 10;
    dev.value[0] = (ultrvalue >> 8) & 0xff;
    dev.value[1] = (ultrvalue) & 0xff;
}

static void ptask_run_callback(ptask_t * ptask)
{
    // 根据状态完成自动订阅
    static data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element, 
        switch ((uintptr_t)element->name)
        {
            case DEV_ELEMENT_NAME_status:
                data_group_get_element_4name2(&group, DEV_ELEMENT_NAME_value)->subscribe = dev.status;
                if (1 == dev.status)
                {
                    zst_timer_resume(timer_handle);
                }
                else
                {
                    zst_timer_pause(timer_handle);
                }
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
            dev.status = 0;
            element_array[0].receive_change_flag = 1;
            break;

        default:
            break;
    }
}


static uint16_t get_ultr_distance(void)
{
    return ultr_get_distance(&ultr);
}

static uint8_t get_gpio_pin_level(struct _ultr * ultr)
{
    return GPIO_ReadPin(ULTR_ECHO_Port, ULTR_ECHO_Pin);
}

static void set_trig_level(struct _ultr * ultr, uint8_t level)
{
    GPIO_WritePin(ULTR_TRIG_Port, ULTR_TRIG_Pin, (GPIO_PinState)level);
}

static uint32_t get_countVal(struct _ultr *ultr)
{
    return CW_BTIM1->CNT;
}
