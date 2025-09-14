#include "data_structure.h"


int data_pack_add_group(data_pack_t * data_pack, data_group_t * data_group)
{
    if (NULL == data_pack || NULL == data_group) return -1;
    data_group->next = NULL;
    if (NULL == data_pack->group_array)
    {
        data_pack->group_array = data_group;
    }
    else
    {
        data_group_t * first_group = data_pack->group_array;
        for ( ; first_group->next != NULL; first_group = first_group->next);
        first_group->next = data_group;
    }
    return 0;
}

data_group_t * data_pack_get_group_4addr(data_pack_t * data_pack, uint8_t addr)
{
    if (NULL==data_pack || NULL==data_pack->group_array) return NULL;

    data_group_t * group;
    DATA_PACK_GROUP_FOREACH(data_pack, group, 
        if (group->addr == addr)
        {
            return group;
        }
    );
    return NULL;
}

data_group_t * data_pack_get_group_4name(data_pack_t * data_pack, char * name)
{
    if (NULL == data_pack || NULL == data_pack->group_array) return NULL;

    data_group_t * group;
    DATA_PACK_GROUP_FOREACH(data_pack, group, 
        if (0 == strcmp(group->name, name))
        {
            return group;
        }
    );
    return NULL;
}

data_element_t * data_group_get_element_4addr(data_group_t * data_group, uint8_t addr)
{
    if (NULL == data_group || 0 == data_group->elements_count) return NULL;
    if (addr >= data_group->elements_count) return NULL;
    return &data_group->elements_array[addr];
}

data_element_t * data_group_get_element_4name(data_group_t * data_group, char * name)
{
    if (NULL == data_group || 0 == data_group->elements_count) return NULL;
    data_element_t * element;

    DATA_GROUP_ELEMENT_FOREACH(data_group, element, 
        if (0 == strcmp(element->name, name))
        {
            return element;
        }
    );
    return NULL;
}

uint32_t data_group_get_size(data_group_t * data_group)
{
    uint32_t size = 0;
    data_element_t * element;
    DATA_GROUP_ELEMENT_FOREACH(data_group, element, 
        size += element->size;
    );
    return size;
}

// 0 成功， -1 失败
int data_element_eliminate_differences(data_group_t * data_group, data_element_t * element)
{
    if (NULL == data_group->comparison_buffer) return -1;
    uintptr_t offset = (uintptr_t)element - (uintptr_t)data_group->elements_array;
    uint8_t index = offset / sizeof(data_element_t);
    // memcmp((uint8_t *)(data_group->comparison_buffer) + index, element->data, element->size);
    memcpy(element->data, (uint8_t *)(data_group->comparison_buffer) + index, element->size);
    return 0;
}

// 0 相等， 1 不相等， -1 有错误
int data_element_campare(data_group_t * data_group, data_element_t * element)
{
    if (NULL == data_group->comparison_buffer) return -1;
    uintptr_t offset = (uintptr_t)element - (uintptr_t)data_group->elements_array;
    uint8_t index = offset / sizeof(data_element_t);
    if (0 == memcmp((uint8_t *)(data_group->comparison_buffer) + index, element->data, element->size))
    {
        return 0;
    }
    return 1;
}

