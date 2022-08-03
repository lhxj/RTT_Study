/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-01     ywx       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "rtconfig.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "mqtt.h"
#include "logic.h"
#include "aht10.h"

void start_rt_thread(void);
static void ReadSensor_Task(void *parameter);
static void StartControl_Task(void *parameter);
static void Mqtt_Task(void *parameter);
