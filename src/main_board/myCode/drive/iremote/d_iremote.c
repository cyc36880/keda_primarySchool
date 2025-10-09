#include "d_iremote.h"
#include "drive/protocol/d_protocol.h"
#include "gpio.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "iremote"

enum DEV_ELEMENT_NAME
{
    DEV_ELEMENT_NAME_status=1,
    DEV_ELEMENT_NAME_count,
    DEV_ELEMENT_NAME_id,
};

typedef struct
{
    uint8_t status;
    uint8_t count;
    uint8_t id;
} d_drive_t;

/****************************
 * function declaration
 ***************************/
static void ptask_event_callback(ptask_t *task, ptask_event_t *e);
static void ptask_run_callback(ptask_t * ptask);
static void ptask_user_message_callback(const ptask_user_message_t *msg);
static uint8_t irm_get_gpio_pin_level(struct _iremote * iremote);
static uint32_t irm_get_countVal(struct _iremote *iremote);


/********************
 * static variables
 *******************/
static d_drive_t dev = {0};
static data_element_t element_array[] = {
    [0] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_status),
        .data = &dev.status,
        .size = sizeof(dev.status),
    },
    [1] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_count),
        .data = &dev.count,
        .size = sizeof(dev.count),
    },
    [2] = {
        .name = DATA_GROUP_UINTPTR_2_NAME(DEV_ELEMENT_NAME_id),
        .data = &dev.id,
        .size = sizeof(dev.id),
    }
};
static uint8_t comparison_buffer[sizeof(dev)] = {0};
static data_group_t group = {
    .name = LOG_TAG,
    .addr = DEVICE_ADDR_IRM,
    .elements_array = element_array,
    .elements_count = sizeof(element_array) / sizeof(element_array[0]),
    .comparison_buffer = comparison_buffer,
};


/********************
 * global variables
 *******************/
iremote_t iremote = {0};


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
    ptask_1_collection.ptask_iremote = ptask_create(ptask_root_1_collection.ptask_root_1, ptask_event_callback, NULL);
    if (NULL == ptask_1_collection.ptask_iremote)
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
    static uint8_t key_count_flag = 0;
    uint8_t remote_val = remote_scan(&iremote);
    if (remote_get_key_count(&iremote) == 0 && key_count_flag > 0)
    {
        key_count_flag = 0;
        dev.id = remote_val;
    }
    if (remote_get_key_count(&iremote)>0 && key_count_flag == 0)
    {
        key_count_flag = 1;
        dev.id = remote_val;
    }

    #if ZST_LOG_LEVEL>0
        if (remote_val)
        {
            ZST_LOG("remote_val = %d", remote_val);
        }
    #endif

    data_element_t * element = NULL;

    // 根据状态完成自动订阅
    DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(&group, element,
        switch ((uintptr_t)element->name)
        {
            case DEV_ELEMENT_NAME_status:
                data_group_get_element_4name2(&group, DEV_ELEMENT_NAME_id)->subscribe = dev.status;
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
            dev.count = 0;
            dev.id = 0;
            element_array[0].receive_change_flag = 1;
            break;

        default:
            break;
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
