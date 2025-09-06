#include "d_protocol.h"

/******************
 * data struct 
 *****************/


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/
static uint8_t udc_protocol_rx_buffer[128] = {0};
static uint8_t udc_protocol_tx_buffer[128] = {0};


/********************
 * global variables
 *******************/
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
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_RECEIVE, udc_protocol_rx_buffer, sizeof(udc_protocol_rx_buffer));
    udc_pack_set_buffer_static(&protocol_pack_KX0, UDC_PACK_TRANSMIT, udc_protocol_tx_buffer, sizeof(udc_protocol_tx_buffer));

    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_root_create(&zst_ptask_list, &task_base);
}

/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    udc_pack_task();
}
