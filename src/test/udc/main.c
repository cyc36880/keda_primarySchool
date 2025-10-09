#include "udc_init.h"

// 停止 all
// aa 55 00 05 00 02 02 01 00 

// 蜂鸣器
// aa 55 00 05 02 02 00 01 00 

// 电机
// aa 55 00 06 05 03 00 1e 1e 00 
int main()
{
    udc_init();

    uint8_t data[] = {0, 1};
    udc_pack_append_data(&pack, 0x04, sizeof(data), data);

    udc_pack_task();
    return 0;
}
