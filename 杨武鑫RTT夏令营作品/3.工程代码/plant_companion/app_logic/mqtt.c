/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-01     ywx       the first version
 */
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 * Again edit by rt-thread group
 * Change Logs:
 * Date          Author          Notes
 * 2019-07-21    MurphyZhao      first edit
 */

#include "rtthread.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "mqtt.h"

//uint8_t temp_f407=0; //采集到的温度数值,-50~100
//uint8_t humi_f407=0; //采集到的湿度数值,0~100

extern rt_uint32_t cur_hum, cur_tem;

char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

int example_subscribe(void *handle)
{
    int res = 0;
    //const char *fmt = "/%s/%s/user/get";
    const char *fmt = "/sys/%s/%s/thing/event/property/post";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

int my_publish(void *handle) //向Topic主题发布属性函数
{
    int             res = 0;
    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    const char     *fmt_payload = "{\"params\" : { \"temperature\":%d,\"humidity\":%d } }";
    char           *topic = NULL;
    int             topic_len = 0;
    int             payload_len = 0;
    char           *payload = NULL;
    payload_len = strlen(fmt_payload)+4;
    payload = HAL_Malloc(payload_len);
        if (payload == NULL) {
            EXAMPLE_TRACE("memory not enough");
            return -1;
        }
        memset(payload, 0, payload_len);
    HAL_Snprintf(payload,payload_len,fmt_payload,cur_tem,cur_hum);
/*    temp_f407 = (temp_f407 + 10)%100;
    humi_f407 = (humi_f407 + 7)%100;*/

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);


    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return -1;
    }

    HAL_Free(topic);
    HAL_Free(payload);
    return 0;
}

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

