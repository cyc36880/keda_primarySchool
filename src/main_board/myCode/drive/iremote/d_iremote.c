#include "d_iremote.h"
#include "drive/protocol/d_protocol.h"
#include "gpio.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "iremote"

typedef struct
{
    uint8_t status;
    uint8_t count;
    uint8_t id;
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static uint8_t irm_get_gpio_pin_level(struct _iremote * iremote);
static uint32_t irm_get_countVal(struct _iremote *iremote);


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
        .name = "count",
        .data = &dev.count,
        .size = sizeof(dev.count),
    },
    [2] = {
        .name = "id",
        .data = &dev.id,
        .size = sizeof(dev.id),
    }
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = "iremote",
    .addr = DEVICE_ADDR_IRM,
    .elements_array = element_array,
    .elements_count = sizeof(element_array) / sizeof(element_array[0]),
    .comparison_buffer = comparison_buffer,
};


/********************
 * global variables
 *******************/
iremote_t iremote;


/********************
 * global functions
 *******************/
void d_iremote_init(void)
{
    /****************
     * 数据包初始化
     ****************/
    data_pack_add_group(&protocol_data_pack_KX0, &group);


    /****************
     * iremote初始化
     **************/
    iremote.get_gpio_pin_level = irm_get_gpio_pin_level;
    iremote.get_countVal = irm_get_countVal;
    iremote_init(&iremote, 0.667, 65535);

    
    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_iremote = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_iremote)
        ZST_LOGE(LOG_TAG, "ptask create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    uint8_t remote_val = remote_scan(&iremote);
    if (remote_val) 
    {
        ZST_LOGD(LOG_TAG, "remote_val: %d\n", remote_val);
        ZST_LOGD(LOG_TAG, "key count : %d\n", remote_get_key_count(&iremote));
    }
}


static uint8_t irm_get_gpio_pin_level(struct _iremote * iremote)
{
    return GPIO_ReadPin(GPIO_IRM_Port, GPIO_IRM_Pin);
}


static uint32_t irm_get_countVal(struct _iremote *iremote)
{
    return CW_BTIM1->CNT;
}
