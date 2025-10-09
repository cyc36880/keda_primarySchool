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
#include "drive/protocol/d_protocol.h"

/******************
 * data struct
 *****************/

 /****************************
 * function declaration
 ***************************/
static void wait_start(void);
static void timer_cb(zst_timer_t *timer);

/********************
 * static variables
 *******************/


 /********************
 * global variables
 *******************/
ptask_root_1_collection_t ptask_root_1_collection = {0};
ptask_1_collection_t ptask_1_collection = {0};


/********************
 * global functions
 *******************/
void setup(void)
{
    zst_init();
    ptask_root_1_collection.ptask_root_1 = ptask_root_create(&zst_ptask_list); // 创建根任务
    ptask_root_select(&zst_ptask_list, ptask_root_1_collection.ptask_root_1); // 选择根任务

    drive_init();
    ZST_LOG("wait on");
    wait_start();
    GPIO_WritePin(GPIO_POWER_EN_Port, GPIO_POWER_EN_Pin, POWER_EN_LEVEL);
    ZST_LOG("power on");
}

void loop(void)
{
    zst_task_handler();
}


/****************************
 * static function
 ***************************/
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
    zst_timer_create(&zst_ztimer, timer_cb, 5, (void *)3);
}

static void timer_cb(zst_timer_t *timer)
{
    uint32_t data = (uint32_t)timer->user_data;
    if (0 == data)
    {
        zst_timer_del(&zst_ztimer, timer);
    }
    else
    {
        udc_pack_push_single(&protocol_pack_KX0, 0, 2, (uint8_t []){1, 1});
        data--;
        timer->user_data = (void *)data;
    }
}
