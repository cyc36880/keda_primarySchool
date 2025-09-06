/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 12:37:20
 * @LastEditTime : 2025-09-06 18:00:11
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */

#include "myMain.h"
#include "drive/drive_init.h"

void setup(void)
{
    zst_init();
    drive_init();
}

void loop(void)
{
    zst_task_handler();
}



