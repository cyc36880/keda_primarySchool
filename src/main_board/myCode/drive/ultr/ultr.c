#include "ultr.h"
#include "sys/delay.h"

void ultr_init(ultr_t * iremote, float tick_ms, uint16_t timer_overflow_val)
{
    iremote->tick_ms = tick_ms;
    iremote->timer_overflow_val = timer_overflow_val;
}

void ultr_timOverFlow_callback(ultr_t * ultr)
{
    ultr->overFlow_count++;
}

void ultr_gpio_interrupt_callback(ultr_t * ultr)
{
    if (1 == ultr->get_echo_pin_level(ultr)) // 捕获到上升沿
    {
        ultr->overFlow_count = 0;
        ultr->get_risingEdge = 1;
        ultr->get_risingEdge_tick = ultr->get_countVal(ultr);
    }
    else // 捕获到下降沿 
    {
        ultr->get_risingEdge = 0;
        uint32_t tick_diff = ultr->get_countVal(ultr) + ultr->overFlow_count * ultr->timer_overflow_val - ultr->get_risingEdge_tick;
        tick_diff = tick_diff * ultr->tick_ms;
        ultr->distance = tick_diff * 17 / 100;
    }
}

uint32_t ultr_get_distance(ultr_t * ultr)
{
    ultr->set_trig_pin_level(ultr, 1);
    delay_ms(1);
    ultr->set_trig_pin_level(ultr, 0);
    
    return ultr->distance;
}



