#ifndef __ULTR_H__
#define __ULTR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

typedef struct _ultr
{
    uint32_t get_risingEdge_tick; // 捕获到上升沿时的tick值
    uint32_t timer_overflow_val;  // 溢出值
    uint16_t overFlow_count;      // 读溢出计数
    uint16_t distance; // 测量到的距离
    uint8_t get_risingEdge : 1;   // 捕获到上升沿
    uint8_t (*get_echo_pin_level)(struct _ultr *);// 获取GPIO电平
    void (*set_trig_pin_level)(struct _ultr *, uint8_t);
    uint32_t (*get_countVal)(struct _ultr *);// 获取计数器时间
    float tick_us; // 一个时钟约几微秒
    uint8_t init_flash : 1; // 是否初始化
} ultr_t;

void ultr_init(ultr_t * ultr, float tick_ms, uint16_t timer_overflow_val);
void ultr_timOverFlow_callback(ultr_t * ultr);
void ultr_gpio_interrupt_callback(ultr_t * ultr);
uint32_t ultr_get_distance(ultr_t * ultr);

#ifdef __cplusplus
}
#endif

#endif /* __ULTR_H__ */
