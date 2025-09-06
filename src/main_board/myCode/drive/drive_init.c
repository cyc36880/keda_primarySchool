/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 17:55:01
 * @LastEditTime : 2025-09-06 21:01:46
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "drive_init.h"
#include "drive/iremote/d_iremote.h"
#include "drive/protocol/d_protocol.h"
#include "drive/buzzer/d_buzzer.h"
#include "drive/misc/d_misc.h"

/******************
 * data struct 
 *****************/


/****************************
 * function declaration
 ***************************/


/********************
 * static variables
 *******************/


/********************
 * global variables
 *******************/


/********************
 * global functions
 *******************/
void drive_init(void)
{
    d_iremote_init();
    d_protocol_init();
    d_buzzer_init();
    d_misc_init();
}


/****************************
 * static function
 ***************************/
