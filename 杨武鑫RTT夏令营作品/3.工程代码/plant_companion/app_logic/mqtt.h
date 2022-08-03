/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-01     ywx       the first version
 */
#ifndef APPLICATIONS_APP_LOGIC_MQTT_H_
#define APPLICATIONS_APP_LOGIC_MQTT_H_

#include "rtthread.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"


void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
int example_subscribe(void *handle);
int my_publish(void *handle); //向Topic主题发布属性函数
void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);


#endif /* APPLICATIONS_APP_LOGIC_MQTT_H_ */
