#include "d_gray.h"
#include "misc/myMath.h"
#include "drive/protocol/d_protocol.h"
#include "adc.h"
#include "tim.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG  "gray"

typedef struct
{
    uint8_t status;   // 1 开启，0 关闭
    uint8_t value[5]; // 5 路灰度值
} d_drive_t;


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static void set_gray_light(uint8_t r, uint8_t g, uint8_t b);

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
    [1] = {
        .name = "value",
        .data = &dev.value,
        .size = sizeof(dev.value),
    },
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
    .addr = DEVICE_ADDR_GRAY,
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
void d_gray_init(void)
{
    set_gray_light(0, 0, 0);
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
    ptask_1_collection.ptask_gray = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_gray)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    dev.value[0] = number_map(adc_get_value(ADC_GRAY_0_CHANNEL), 0, 4095 + 1, 0, 255 + 1);
    dev.value[1] = number_map(adc_get_value(ADC_GRAY_1_CHANNEL), 0, 4095 + 1, 0, 255 + 1);
    dev.value[2] = number_map(adc_get_value(ADC_GRAY_2_CHANNEL), 0, 4095 + 1, 0, 255 + 1);
    dev.value[3] = number_map(adc_get_value(ADC_GRAY_3_CHANNEL), 0, 4095 + 1, 0, 255 + 1);
    dev.value[4] = number_map(adc_get_value(ADC_GRAY_4_CHANNEL), 0, 4095 + 1, 0, 255 + 1);

    // 根据状态完成自动订阅
    static data_element_t * element = NULL;
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element, 
        if (0 == strcmp(element->name, "status"))
        {
            data_group_get_element_4name(&group, "value")->subscribe = dev.status;
            if (dev.status)
                set_gray_light(200, 200, 200);
            else
                set_gray_light(0, 0, 0);
        }
    );
}



static void set_gray_light(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t r_val = number_map(r, 0, 255 + 1, 0, 1000 + 1);
    uint16_t g_val = number_map(g, 0, 255 + 1, 0, 1000 + 1);
    uint16_t b_val = number_map(b, 0, 255 + 1, 0, 1000 + 1);

    ATIM_SetCompare1A(1000 - r_val);
    ATIM_SetCompare3B(1000 - g_val);
    ATIM_SetCompare2B(1000 - b_val);
}

