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
        .subscribe = 1, // 默认标记为订阅
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
    set_gray_light(0, 255, 255);

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
    
}



static void set_gray_light(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t r_val = number_map(r, 0, 255 + 1, 0, 1000 + 1);
    uint16_t g_val = number_map(g, 0, 255 + 1, 0, 1000 + 1);
    uint16_t b_val = number_map(b, 0, 255 + 1, 0, 1000 + 1);

    ATIM_SetCompare1A(r_val);
    ATIM_SetCompare2A(g_val);
    ATIM_SetCompare3A(b_val);
}

