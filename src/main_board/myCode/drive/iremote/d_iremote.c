#include "d_iremote.h"

#include "gpio.h"

/******************
 * data struct 
 *****************/


/****************************
 * function declaration
 ***************************/
static void ptask_run_callback(ptask_t * ptask);
static uint8_t irm_get_gpio_pin_level(struct _iremote * iremote);
static uint32_t irm_get_countVal(struct _iremote *iremote);

/********************
 * static variables
 *******************/


/********************
 * global variables
 *******************/
iremote_t iremote;


/********************
 * global functions
 *******************/
void d_iremote_init(void)
{
    /****************
     * iremote初始化
     **************/
    iremote.get_gpio_pin_level = irm_get_gpio_pin_level;
    iremote.get_countVal = irm_get_countVal;
    iremote_init(&iremote, 1.5, 10000);

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
    uint8_t remote_val = remote_scan(&iremote);
    if (remote_val) 
    {
        printf("remote_val: %d\n", remote_val);
    }
}


static uint8_t irm_get_gpio_pin_level(struct _iremote * iremote)
{
    return GPIO_ReadPin(GPIO_IRM_Port, GPIO_IRM_Pin);
}


static uint32_t irm_get_countVal(struct _iremote *iremote)
{
    return CW_BTIM1->CNT;
}
