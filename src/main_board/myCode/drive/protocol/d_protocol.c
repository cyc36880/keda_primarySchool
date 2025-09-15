#include "d_protocol.h"
#include "sys/delay.h"
#include "uart.h"

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

typedef struct
{
    uint8_t need_ack : 1;
    uint8_t id_100_cmd : 7;
} protocol_command_t;


/****************************
 * function declaration
 ***************************/
static int udc_send_bytes_callback(const struct _udc_pack_t *pack, const uint8_t *buf, uint16_t len);
static void ptask_run_callback(ptask_t * ptask);
static void udc_event_receive_finsh(udc_event_t * e);
static void subscribe_loop(void);

/********************
 * static variables
 *******************/
static protocol_command_t protocol_command = {0};
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
            .header = "KX0",
            .header_len = 3,
        }
    };
    udc_pack_init(&init);
    udc_pack_set_send_bytes_func(&protocol_pack_KX0, udc_send_bytes_callback);
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_RECEIVE, udc_protocol_rx_buffer, sizeof(udc_protocol_rx_buffer));
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_TRANSMIT, udc_protocol_tx_buffer, sizeof(udc_protocol_tx_buffer));

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

// 订阅后，主动查找与上一次的数据不同的数据，并通知主机
static void subscribe_loop(void)
{
    static data_group_t * group;
    static data_element_t * element;
    static uint8_t foreach_count=0;
    static uint32_t buffer_index = 0;
    static uint32_t buffer_size = 0;
    static uint8_t * buffer = NULL;
    uint8_t append_data_count = 0; // 向pack中append数据的次数

    udc_pack_clear(&protocol_pack_KX0, UDC_PACK_TRANSMIT); // 清空发送缓冲区
    udc_pack_append_data(&protocol_pack_KX0, PACK_IDENTIFY_ID, 1, (uint8_t []){11}); // 添加命令
    DATA_PACK_GROUP_FOREACH(&protocol_data_pack_KX0, group,
        foreach_count=0;
        buffer_index = 0;
        buffer_size = 0;
        buffer = NULL;
        // 查找需要发送的数据
        DATA_GROUP_ELEMENT_FOREACH(group, element,
            if (1 == element->subscribe && 1 == data_element_campare(group, element))
            {
                data_element_eliminate_differences(group, element); // 消除差异
                element->send_flag = 1; // 标记需要发送
                buffer_size += 2 + element->size;
            }
        );
        if (0 == buffer_size) continue;
        // 分配发送缓冲区
        buffer = (uint8_t *)zst_mem_alloc(buffer_size);
        if (NULL == buffer) 
        {
            ZST_LOGE(LOG_TAG, "subscribe_loop alloc buffer failed!");
            return;
        }
        // 填充发送缓冲区
        DATA_GROUP_ELEMENT_FOREACH(group, element,
            if (1 == element->send_flag)
            {
                element->send_flag = 0; // 清空标记
                buffer[buffer_index] = foreach_count;
                buffer[buffer_index + 1] = element->size;
                memcpy(&buffer[buffer_index + 2], element->data, element->size);
                ZST_LOG("element_name %s, size %d", element->name, element->size);
            }
            foreach_count++;
            buffer_index += (uintptr_t)element->data - (uintptr_t)group->elements_array;
        );
        if (0 != udc_pack_append_data(&protocol_pack_KX0, group->addr, buffer_size, buffer)) // 内部可能填充满了
        {
            append_data_count = 0;
            udc_pack_push(&protocol_pack_KX0); // 
            delay_ms(10); // 分包发送需要一定间隔，避免主机处理不及时丢失数据
            udc_pack_append_data(&protocol_pack_KX0, PACK_IDENTIFY_ID, 1, (uint8_t []){11}); // 添加命令
            udc_pack_append_data(&protocol_pack_KX0, group->addr, buffer_size, buffer);
        }
        else
        {
            append_data_count++;
        }
        zst_mem_free(buffer);
        buffer = NULL;
    );
    if (append_data_count > 0)
        udc_pack_push(&protocol_pack_KX0); // 
    else
        udc_pack_clear(&protocol_pack_KX0, UDC_PACK_TRANSMIT); // 清空发送缓冲区
}

// 主机向从机发送数据
static int protocol_command_give(const udc_pack_t *pack)
{
    udc_obj_t obj;
    UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, pack, &obj, 
        if (obj.id >= 100) continue;
        data_group_t * group = data_pack_get_group_4addr(&protocol_data_pack_KX0, obj.id);
        if (NULL == group) return -1;
        if (obj.size < 3) return -1;
        for (uint8_t data_start = 0; data_start < obj.size; )
        {
            uint8_t id = obj.data[data_start];
            uint8_t size = obj.data[data_start + 1];
            uint8_t * data = &obj.data[data_start + 2];

            if (data_start + 2 + size > obj.size) return -1;

            data_element_t * element = data_group_get_element_4addr(group, id);
            if (NULL == element) return -1;
            if (size > element->size) return -1;
            memcpy(element->data, data, size);
            element->receive_change_flag = 1;
            data_start += 2 + size;
            data_element_eliminate_differences(group, element); // 消除差异，避免将主机发送过来的数据再次发送给主机
        }
    );
    return 0;
}

// 主机向从机请求数据
static int protocol_command_get(udc_pack_t *pack)
{
    uint8_t append_data_count = 0; // 向pack中append数据的次数
    udc_pack_clear(pack, UDC_PACK_TRANSMIT); // 清空发送缓冲区
    udc_pack_append_data(pack, PACK_IDENTIFY_ID, 1, (uint8_t []){10}); // 添加命令
    udc_obj_t obj;
    UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, pack, &obj, 
        if (obj.id >= 100) continue;
        data_group_t * group = data_pack_get_group_4addr(&protocol_data_pack_KX0, obj.id);
        if (NULL == group) return -1;
        if (0 == obj.size) // 全部
        {
            ZST_LOG("get all data");
            uint32_t buffer_size = data_group_get_size(group) + group->elements_count * 2;
            uint8_t * buffer = (uint8_t *)zst_mem_alloc(buffer_size);
            if (NULL == buffer) return -1;
            data_element_t * element;
            uint32_t buffer_index = 0;
            uint8_t foreach_count=0;
            DATA_GROUP_ELEMENT_FOREACH(group, element, 
                buffer[buffer_index] = foreach_count++;
                buffer[buffer_index + 1] = element->size;
                memcpy(&buffer[buffer_index + 2], element->data, element->size);
                buffer_index += 2 + element->size;
            );
            udc_pack_append_data(pack, obj.id, buffer_size, buffer);
            zst_mem_free(buffer);
            continue;
        }
        else // 指定
        {
            ZST_LOG("get some data");
            data_element_t * element = NULL;
            uint32_t buffer_size = 0;
            uint32_t buffer_index = 0;
            for (uint16_t i = 0; i < obj.size; i++) // 计算所需的数据大小
            {
                element = data_group_get_element_4addr(group, obj.data[i]);
                if (NULL == element) return -1;
                buffer_size += 2 + element->size;
            }
            uint8_t * buffer = (uint8_t *)zst_mem_alloc(buffer_size);
            if (NULL == buffer) return -1;
            for (uint16_t i = 0; i < obj.size; i++) // 填充数据
            {
                element = data_group_get_element_4addr(group, obj.data[i]);
                buffer[buffer_index] = obj.data[i]; // id
                buffer[buffer_index + 1] = element->size; // size
                memcpy(&buffer[buffer_index + 2], element->data, element->size); // data
                buffer_index += 2 + element->size;
            }
            if (0 != udc_pack_append_data(pack, obj.id, buffer_size, buffer)) // 内部可能填充满了
            {
                append_data_count = 0;
                udc_pack_push(pack); // 
                delay_ms(10); // 分包发送需要一定间隔，避免主机处理不及时丢失数据
                udc_pack_append_data(pack, PACK_IDENTIFY_ID, 1, (uint8_t []){10}); // 添加命令
                udc_pack_append_data(pack, obj.id, buffer_size, buffer);
            }
            else
            {
                append_data_count ++;
            }
            zst_mem_free(buffer);
        }
    );
    if (append_data_count > 0)
    {
        udc_pack_push(pack);
    }
    else
    {
        udc_pack_clear(pack, UDC_PACK_TRANSMIT); // 清空发送缓冲区
    }
    return 0;
}

// 订阅设备
static int subscribe_device_4_udcpack(const udc_pack_t *pack, bool en)
{
    udc_obj_t obj;
    UDC_PACK_OBJ_FOREACH(UDC_PACK_RECEIVE, pack, &obj, 
        if (obj.id >= 100) continue;
        data_group_t * group = data_pack_get_group_4addr(&protocol_data_pack_KX0, obj.id);
        if (NULL == group) return -1;
        if (0 == obj.size) // 全部 订阅/取消订阅
        {
            data_element_t * element;
            DATA_GROUP_ELEMENT_FOREACH(group, element, 
                element->subscribe = en;
            );
            continue;
        }
        for (uint8_t i = 0; i < obj.size; i++)
        {
            data_element_t * element = data_group_get_element_4addr(group, obj.data[i]);
            if (NULL == element) return -1;
            element->subscribe = en;
        }
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
    // 是否需要应答
    udc_obj_t obj;
    if (0 == udc_pack_get_obj(&protocol_pack_KX0, UDC_PACK_RECEIVE, 101, &obj))
    {
        if (1 == obj.size)
        {
            switch (obj.data[0])
            {
                case 0:
                    ZST_LOG("no ack");
                    protocol_command.need_ack = 0;
                    break;
                case 1:
                    ZST_LOG("need ack");
                    protocol_command.need_ack = 1;
                    break;
                default:
                    break;
            }
        }
    }

    // 获取 id 100 数据识别包
    if (0 != udc_pack_get_obj(&protocol_pack_KX0, UDC_PACK_RECEIVE, 100, &obj))
    {
        ZST_LOGE(LOG_TAG, "NO id 100 obj!");
        D_PROTOCOL_PUSH_ACK(PROTOCOL_ACK_MISSING_ID_100);
        return;
    }

    if (1 == obj.size)
    {
        protocol_command.id_100_cmd = obj.data[0];
        if (protocol_command.id_100_cmd > 4) // 是否支持该命令
        {
            D_PROTOCOL_PUSH_ACK(PROTOCOL_ACK_ID_100_NOSUPPORT_CMD);
            return;
        }
    }

    switch (protocol_command.id_100_cmd)
    {
        case 0: // 响应
            break;
        case 1: // 给予命令
            if (0 != protocol_command_give(&protocol_pack_KX0))
            {
                D_PROTOCOL_PUSH_ACK(PROTOCOL_ACK_ERROR);
            }
            break;
        case 2: // 请求命令
            if (0 != protocol_command_get(&protocol_pack_KX0))
            {
                udc_pack_clear(&protocol_pack_KX0, UDC_PACK_TRANSMIT); // 清空发送缓冲区
                D_PROTOCOL_PUSH_ACK(PROTOCOL_ACK_ERROR);
            }
            break;
        case 3: // 订阅设备
        case 4: // 取消订阅设备
            if (0 != subscribe_device_4_udcpack(&protocol_pack_KX0, protocol_command.id_100_cmd == 3))
            {
                D_PROTOCOL_PUSH_ACK(PROTOCOL_ACK_ERROR);
                return;
            }
            break;
    }
    protocol_command.need_ack = 0;
}


