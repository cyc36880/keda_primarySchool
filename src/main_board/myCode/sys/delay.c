#include "delay.h"

void delay_ms(uint32_t ms)
{
    uint32_t last_tick = zst_tick_get();
    while (zst_tick_elaps(last_tick) < ms)
        ;
}
