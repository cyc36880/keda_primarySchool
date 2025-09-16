/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-12 17:32:07
 * @LastEditTime : 2025-09-15 20:37:01
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_ultr.h"
#include "drive/protocol/d_protocol.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "ultr"

typedef struct
{
    uint8_t status;
    uint8_t value[2];
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static uint16_t get_ultr_distance(void);
static uint8_t get_gpio_pin_level(struct _ultr * iremote);
static void set_trig_level(struct _ultr * ultr, uint8_t level);
static uint32_t get_countVal(struct _ultr *iremote);

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
    [2] = {
        .name = "value",
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
ultr_t ultr = {
    .get_echo_pin_level = get_gpio_pin_level,
    .get_countVal = get_countVal,
    .set_trig_pin_level = set_trig_level,
};

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
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_ultr = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_ultr)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    // 根据状态完成自动订阅
    static data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element, 
        if (0 == strcmp(element->name, "status"))
        {
            data_group_get_element_4name(&group, "value")->subscribe = dev.status;
        }
    );

    uint16_t value = get_ultr_distance();
    dev.value[0] = value >> 8;
    dev.value[1] = value & 0xff;
}

static uint16_t get_ultr_distance(void)
{
    // return ultr_get_distance(&ultr);
    return 0;
}

static uint8_t get_gpio_pin_level(struct _ultr * ultr)
{
    return GPIO_ReadPin(ULTR_ECHO_Port, ULTR_ECHO_Pin);
}

static void set_trig_level(struct _ultr * ultr, uint8_t level)
{
    GPIO_WritePin(ULTR_TRIG_Port, ULTR_TRIG_Pin, level);
}

static uint32_t get_countVal(struct _ultr *ultr)
{
    return CW_BTIM1->CNT;
}
