#include "d_protocol.h"
#include "sys/delay.h"
#include "uart.h"
#include "misc/myMath.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG "protocol"

#define D_PROTOCOL_PUSH_ACK(_ack)                                            \
    do                                                                       \
    {                                                                        \
        if (0 == protocol_command.need_ack)                                  \
            break;                                                           \
        uint8_t ack = (_ack);                                                \
        udc_pack_append_data(&protocol_pack_KX0, PACK_IDENTIFY_ID, 1, &ack); \
    }                                                                        \
    while(0)




/****************************
 * function declaration
 ***************************/
static int udc_send_bytes_callback(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len);
static void ptask_run_callback(ptask_t * ptask);
static void udc_event_receive_finsh(udc_event_t * e);
static void subscribe_loop(void);
static int calculate_verify_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify);

/********************
 * static variables
 *******************/
static udc_event_dsc_t udc_event_receive_finsh_des = {0};
static uint8_t udc_protocol_rx_buffer[128] = {0};
static uint8_t udc_protocol_tx_buffer[128] = {0};


/********************
 * global variables
 *******************/
data_pack_t protocol_data_pack_KX0 = {0};
udc_pack_t protocol_pack_KX0 = {0};


/********************
 * global functions
 *******************/
void d_protocol_init(void)
{
    /****************
     * 协议初始化
     ***************/
    udc_pack_init_t init = {
        .pack = &protocol_pack_KX0,
        .header = {
            .header = "\xAA\x55",
            .header_len = 2,
        }
    };
    udc_pack_init(&init);
    udc_pack_set_send_bytes_func(&protocol_pack_KX0, udc_send_bytes_callback);
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_RECEIVE, udc_protocol_rx_buffer, sizeof(udc_protocol_rx_buffer));
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_TRANSMIT, udc_protocol_tx_buffer, sizeof(udc_protocol_tx_buffer));
    udc_pack_set_calculate_verify_func(&protocol_pack_KX0, 1, calculate_verify_func); // 重定向校验

    /****************
     * 协议事件注册
     ***************/
    udc_pack_add_event_cb_static(&protocol_pack_KX0, &udc_event_receive_finsh_des, udc_event_receive_finsh, UDC_EVENT_PACK_RECEIVE_FINSHED, NULL);


    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_protocol = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_protocol)
        ZST_LOGE(LOG_TAG, "ptask_protocol create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask_protocol create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    subscribe_loop();
    udc_pack_task();
}

static int udc_send_bytes_callback(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len)
{
    uart_send_data(&uart1, (uint8_t *)buf, len);
    return len;
}

static int calculate_verify_func(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len, uint8_t *verify)
{
    *verify = 0;
    return 0;
}

// 订阅后，主动查找与上一次的数据不同的数据，并通知主机
static void subscribe_loop(void)
{
    static data_group_t * group;
    static data_element_t * element;
    static uint8_t foreach_count=0;

    udc_pack_clear(&protocol_pack_KX0, UDC_PACK_TRANSMIT); // 清空发送缓冲区
    DATA_PACK_GROUP_FOREACH(&protocol_data_pack_KX0, group,
        foreach_count = 0;
        // 查找需要发送的数据
        DATA_GROUP_ELEMENT_FOREACH(group, element,
            if (1 == element->subscribe && 1 == data_element_campare(group, element))
            {
                data_element_eliminate_differences(group, element); // 消除差异
                element->send_flag = 1; // 标记需要发送
            }
        );
        // 填充发送缓冲区
        DATA_GROUP_ELEMENT_FOREACH(group, element,
            if (1 == element->send_flag)
            {
                element->send_flag = 0; // 清空标记
                uint16_t buffer_size = element->size + 1;
                uint8_t * buffer = zst_mem_alloc(buffer_size);
                if (NULL == buffer)
                {
                    ZST_LOGE(LOG_TAG, "file alloc");
                    return;
                }
                buffer[0] = foreach_count;
                memcpy(&buffer[1], element->data, element->size);
                udc_pack_append_data(&protocol_pack_KX0, group->addr, buffer_size, buffer);
                udc_pack_push(&protocol_pack_KX0);
                zst_mem_free(buffer);
            }
            foreach_count++;
        );
    );
}

// 主机向从机发送数据
static int protocol_command_give(const udc_pack_t *pack)
{
    udc_obj_t obj;
    UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, pack, &obj, 
        data_group_t * group = data_pack_get_group_4addr(&protocol_data_pack_KX0, obj.id);
        if (NULL == group) return -1;
        
        uint8_t id = obj.data[0];
        uint8_t * data = &obj.data[1];
        uint8_t size =obj.size - 1;

        data_element_t * element = data_group_get_element_4addr(group, id);
        if (NULL == element) return -1;
        memcpy(element->data, data, min(size, element->size));
        element->receive_change_flag = 1;
        // data_element_eliminate_differences(group, element); // 消除差异，避免将主机发送过来的数据再次发送给主机
    );
    return 0;
}


// udc 接收完成回调
static void udc_event_receive_finsh(udc_event_t * e)
{
    #if DEF_CLOG_DEFAULT_LEVEL>=4
        udc_obj_t _obj;
        UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, &protocol_pack_KX0, &_obj, 
            ZST_LOG("pack receive >> id: %d, size: %d", _obj.id, _obj.size);
        );
    #endif

    // TODO: 解析数据
    protocol_command_give(&protocol_pack_KX0);
}


