#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

/**
 *   pack(addr1) --- pack(addr1) --- ...
 *     |
 *  element(addr2)
 *     |
 *  element(addr2)
 *    ...
 */

#define DATA_PACK_GROUP_FOREACH(_data_pack_p, _data_group_p, active)                                                \
    do                                                                                                              \
    {                                                                                                               \
        for (_data_group_p = (_data_pack_p)->group_array; _data_group_p != NULL; _data_group_p = _data_group_p->next) \
        {                                                                                                           \
            active                                                                                                  \
        }                                                                                                           \
    } while (0)

#define DATA_GROUP_ELEMENT_FOREACH(_data_group_p, _data_element_p, active)                      \
    do                                                                                          \
    {                                                                                           \
        for (uint16_t _element_n = 0; _element_n < _data_group_p->elements_count; _element_n++) \
        {                                                                                       \
            _data_element_p = &_data_group_p->elements_array[_element_n];                       \
            active                                                                              \
        }                                                                                       \
    } while (0)

#define DATA_GROUP_ELEMENT_CHACK_CHANGE_FOREACH(_data_group_p, _data_element_p, active)         \
    do                                                                                          \
    {                                                                                           \
        for (uint16_t _element_n = 0; _element_n < (_data_group_p)->elements_count; _element_n++) \
        {                                                                                       \
            _data_element_p = &(_data_group_p)->elements_array[_element_n];                       \
            if (1 == _data_element_p->receive_change_flag)                                      \
            {                                                                                   \
                _data_element_p->receive_change_flag = 0;                                       \
                active                                                                          \
            }                                                                                   \
        }                                                                                       \
    } while (0)

typedef struct _data_element
{
    char * name;
    void * data;
    uint16_t size;
    uint8_t receive_change_flag : 1;
    uint8_t send_flag : 1; // 发送
    uint8_t subscribe : 1; // 订阅
} data_element_t;

typedef struct _data_group
{
    struct _data_group * next;
    char * name;
    uint8_t addr;
    data_element_t * elements_array;
    uint16_t elements_count;
    void * comparison_buffer;// 大小与elements_array一直，用于比较
} data_group_t;

typedef struct _data_pack
{
    data_group_t * group_array;
} data_pack_t;

int data_pack_add_group(data_pack_t * data_pack, data_group_t * data_group);

data_group_t * data_pack_get_group_4addr(data_pack_t * data_pack, uint8_t addr);
data_group_t * data_pack_get_group_4name(data_pack_t * data_pack, char * name);

data_element_t * data_group_get_element_4addr(data_group_t * data_group, uint8_t addr);
data_element_t * data_group_get_element_4name(data_group_t * data_group, char * name);

uint32_t data_group_get_size(data_group_t * data_group);
int data_element_eliminate_differences(data_group_t * data_group, data_element_t * element);
int data_element_campare(data_group_t * data_group, data_element_t * element);

#ifdef __cplusplus
}
#endif

#endif /* __DATA_STRUCTURE_H__ */
