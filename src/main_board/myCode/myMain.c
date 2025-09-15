/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 12:37:20
 * @LastEditTime : 2025-09-15 09:46:33
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */

#include "myMain.h"
#include "drive/drive_init.h"

ptask_root_1_collection_t ptask_root_1_collection = {0};
ptask_1_collection_t ptask_1_collection = {0};

void setup(void)
{
    zst_init();
    ptask_root_1_collection.ptask_root_1 = ptask_root_create(&zst_ptask_list, NULL); // 创建根任务
    drive_init();
}

void loop(void)
{
    zst_task_handler();
}



