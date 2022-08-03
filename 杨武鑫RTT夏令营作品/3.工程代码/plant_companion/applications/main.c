/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "rtconfig.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "app_logic/mqtt.h"
#include "app_logic/logic.h"
#include "aht10.h"
#include "app_logic/app_init.h"

/* defined the LED_GREEN pin: PD4 */
#define LED_GREEN_PIN GET_PIN(D, 4)
#define LED0_PIN GET_PIN(D, 3)
#define LED2_PIN GET_PIN(D, 5)
#define LED3_PIN GET_PIN(D, 6)

//¼ÌµçÆ÷
#define RELAY_PIN GET_PIN(C, 6)

int main(void)
{
    start_rt_thread();
    /* set LED_GREEN_PIN pin mode to output */
    rt_pin_mode(LED_GREEN_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(RELAY_PIN, PIN_MODE_OUTPUT);
    while (1)
    {
        rt_pin_write(LED_GREEN_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
    }
}
