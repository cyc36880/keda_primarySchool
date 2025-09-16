/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : ZIShen
 * @Date         : 2025-09-06 17:55:01
 * @LastEditTime : 2025-09-15 18:11:11
 * @Description  : 
 * Copyright (c) 2025 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */
#include "drive_init.h"
#include "drive/sys/d_sys.h"
#include "drive/iremote/d_iremote.h"
#include "drive/protocol/d_protocol.h"
#include "drive/buzzer/d_buzzer.h"
#include "drive/misc/d_misc.h"
#include "drive/motor/d_motor.h"
#include "drive/gray/d_gray.h"
#include "drive/ultr/d_ultr.h"


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
    d_protocol_init();
    d_ultr_init();
    d_iremote_init();
    d_sys_init();
    d_buzzer_init();
    d_misc_init();
    d_motor_init();
    
    d_gray_init();
}


/****************************
 * static function
 ***************************/
