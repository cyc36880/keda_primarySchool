#include "d_misc.h"



/******************
 * data struct 
 *****************/


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
void d_misc_init(void)
{
    /****************
     * 任务初始化
     ****************/
    ptask_base_t task_base = {
        .run = ptask_run_callback
    };
    ptask_root_create(&zst_ptask_list, &task_base);
}


/****************************
 * static function
 ***************************/
static void ptask_run_callback(ptask_t * ptask)
{

}
#
