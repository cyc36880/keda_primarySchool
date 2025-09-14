#include "udc_init.h"



int main()
{
    udc_init();

    uint8_t data[] = {4};
    udc_pack_append_data(&pack, 100, 1, data);

    // uint8_t ack = 1;
    // udc_pack_append_data(&pack, 101, 1, &ack);

    uint8_t send_buf[] = {1, 3};
    udc_pack_append_data(&pack, 5, 1, send_buf);

    udc_pack_task();
    return 0;
}
