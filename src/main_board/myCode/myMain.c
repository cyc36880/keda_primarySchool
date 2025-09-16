/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑     永不宕机     永无BUG
 */

/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 12:37:20
 * @LastEditTime : 2025-09-15 17:29:54
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */

#include "myMain.h"
#include "drive/drive_init.h"

static void wait_start(void);

ptask_root_1_collection_t ptask_root_1_collection = {0};
ptask_1_collection_t ptask_1_collection = {0};

void setup(void)
{
    zst_init();
    ptask_root_1_collection.ptask_root_1 = ptask_root_create(&zst_ptask_list, NULL); // 创建根任务
    drive_init();
    ZST_LOG("wait on");
    wait_start();
    ZST_LOG("power on");
}

void loop(void)
{
    zst_task_handler();
}



static void wait_start(void)
{
    uint32_t last_tick = zst_tick_get();
    while (1)
    {
        if (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
        {
            if (zst_tick_elaps(last_tick) > POWER_ON_PRESS_MS)
                break;
        }
        else
        {
            last_tick = zst_tick_get();
        }
    }
    GPIO_WritePin(GPIO_POWER_EN_Port, GPIO_POWER_EN_Pin, POWER_EN_LEVEL);
    zst_task_handler(); // 运行一次主程序，以发送一次开机指令
    while (GPIO_ReadPin(GPIO_POWER_Port, GPIO_POWER_Pin) == 0)
        ;
}
