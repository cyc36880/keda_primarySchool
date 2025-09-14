/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 20:54:06
 * @LastEditTime : 2025-09-12 13:51:49
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "d_buzzer.h"

/******************
 * data struct 
 *****************/
#define LOG_TAG  "buzzer"

/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);


/********************
 * static variables
 *******************/


/********************
 * global variables
 *******************/


/********************
 * global functions
 *******************/
void d_buzzer_init(void)
{
    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_1_collection.ptask_buzzer = ptask_create(ptask_root_1_collection.ptask_root_1, &task_base);
    if (NULL == ptask_1_collection.ptask_buzzer)
        ZST_LOGE(LOG_TAG, "ptask_buzzer create failed!");
    else
        ZST_LOGI(LOG_TAG, "ptask_buzzer create success!");
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{
    
}
