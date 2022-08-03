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
#include "sensor.h"
#include "sensor_dallas_dht11.h"
#include "drv_gpio.h"

/* ������ƿ� */
struct rt_mailbox tem_mb;
struct rt_mailbox hum_mb;
/* ���ڷ��ʼ����ڴ�� */
char mb_hum_pool[128];
char mb_tem_pool[128];


/* ָ���ź�����ָ�� */
static rt_sem_t dynamic_sem = RT_NULL;

/* ָ�򻥳�����ָ�� */
static rt_mutex_t dynamic_mutex = RT_NULL;

#define TASK_TIMESLICE     5

extern char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1];
extern char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1];
extern char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1];

extern rt_adc_device_t adc_dev;

//1
/***************������������*************/
#define SENSOR_PRIORITY                         12
#define SENSOR_TASK_SIZE                        4096
rt_sem_t sensor_data_sem = RT_NULL;
static rt_thread_t sensor_task_thread = RT_NULL;
rt_uint32_t hum;
rt_uint32_t tem;
/*���ݶ�ȡ�߳���ں��� */
static void ReadSensor_Task(void *parameter);
/***************������������*************/


//2
/***************����������*************/
#define CONTROL_PRIORITY                        14
#define CONTROL_TASK_SIZE                       4096
static rt_thread_t control_task_thread = RT_NULL;
/*������������߳���ں��� */
static void StartControl_Task(void *parameter);
/***************����������*************/

//3
/***************����MQTT�߳�*************/
#define MQTT_PRIORITY                        16
#define MQTT_TASK_SIZE                       4096
static rt_thread_t MQTT_task_thread = RT_NULL;
/*Mqtt�߳���ں��� */
static void Mqtt_Task(void *parameter);
/***************����MQTT�߳�*************/

/*�̴߳�������*/
int start_rt_thread(void)
{
    rt_err_t result;

//     ��ʼ��һ�� hum_mailbox
    result = rt_mb_init(&hum_mb,
                        "hum_mbt",                      // ������ tem_mbt
                        &mb_hum_pool[0],                // �����õ����ڴ���� mb_hum_pool
                        sizeof(mb_hum_pool) / 4,        // �����е��ʼ���Ŀ����Ϊһ���ʼ�ռ 4 �ֽ�
                        RT_IPC_FLAG_FIFO);          // ���� FIFO ��ʽ�����̵߳ȴ�
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
    }

   //  ��ʼ��һ�� tem_mailbox   RT_SERIAL_RB_BUFSZ
    result = rt_mb_init(&tem_mb,
                        "tem_mbt",                       //������ tem_mbt
                        &mb_tem_pool[0],                 //�����õ����ڴ���� mb_tem_pool
                        sizeof(mb_tem_pool) / 4,         //�����е��ʼ���Ŀ����Ϊһ���ʼ�ռ 4 �ֽ�
                        RT_IPC_FLAG_FIFO);           //���� FIFO ��ʽ�����̵߳ȴ�
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
    }


  //   ����һ����̬�ź�������ʼֵ��0

    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);
    if (dynamic_sem == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore failed.\n");
        return -1;
    }
    else
    {
        rt_kprintf("create done. dynamic semaphore value = 0.\n");
    }


//     ����һ����̬������
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_PRIO);
    if (dynamic_mutex == RT_NULL)
    {
        rt_kprintf("create dynamic mutex failed.\n");
        return -1;
    }


//     1�����������߳�
    sensor_task_thread = rt_thread_create("sensor_th",
                                          ReadSensor_Task, RT_NULL,
                                          SENSOR_TASK_SIZE,
                                          SENSOR_PRIORITY, TASK_TIMESLICE);

  //   �������߳̿��ƿ�,��������߳�
    if (sensor_task_thread != RT_NULL)
        rt_thread_startup(sensor_task_thread);

 //    2���������߳�
    control_task_thread = rt_thread_create("con_th",
                                           StartControl_Task, RT_NULL,
                                           CONTROL_TASK_SIZE,
                                           CONTROL_PRIORITY, TASK_TIMESLICE);

  //   �������߳̿��ƿ�,��������߳�
    if (control_task_thread != RT_NULL)
        rt_thread_startup(control_task_thread);


//     3����MQTT�߳�
    MQTT_task_thread = rt_thread_create("mqtt_th",
                                        Mqtt_Task, RT_NULL,
                                        CONTROL_TASK_SIZE,
                                        MQTT_PRIORITY, TASK_TIMESLICE);
    if (MQTT_task_thread != RT_NULL)
        rt_thread_startup(MQTT_task_thread);

    rt_thread_mdelay(2000);
    return 0;
}

/*���ݶ�ȡ�߳���ں���*/
static void ReadSensor_Task(void *parameter)
{
    access_Sensor();
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
    rt_size_t res;
    rt_uint8_t get_data_freq = 1;  //1Hz
    dev = rt_device_find("temp_dht11");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));

    while(1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);
        if (sensor_data.data.temp >= 0)
        {
            uint8_t temp = (sensor_data.data.temp & 0xffff) >> 0;      // get temp
            tem = (rt_uint32_t)temp;
            //uint8_t humi = (sensor_data.data.temp & 0xffff0000) >> 16; // get humi
        }
        rt_kprintf("enter the ReadSensor_Task\n");

        //tem = (rt_uint32_t)aht10_read_temperature(aht_dev);
        hum = (rt_uint32_t)rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
        hum = (rt_uint32_t)((6092 - hum) * 100) / 3292;
        rt_kprintf("the hum = %d%, the tem = %d\r\n", hum, tem);
        //rt_kprintf("tem:%d\n" ,tem);

        //��ȡ������,���õ����¶Ⱥ�ʪ����ֵ���͵�����
        rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
        rt_mb_send(&tem_mb, tem);
        rt_thread_mdelay(10);
        rt_mb_send(&hum_mb, hum);
        rt_kprintf("send the mailbox\n");
        rt_mutex_release(dynamic_mutex);

        //�ͷ��ź���
        rt_sem_release(dynamic_sem);
        rt_thread_mdelay(1000);
    }
}
/*������������߳���ں��� */
static void StartControl_Task(void *parameter)
{
    static rt_err_t result;
    while(1)
    {
        result = rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);
        if (result != RT_EOK)
               {
                   rt_kprintf("t2 take a dynamic semaphore, failed.\n");
                   rt_sem_delete(dynamic_sem);
                   return;
               }
               else
               {
                   Sensor_Logic_Running();
                   rt_kprintf("enter the Sensor_Logic_Running\n");
               }
        rt_sem_release(dynamic_sem);
        rt_thread_mdelay(500);
    }
}

/*MQTT�߳���ں��� */
static void Mqtt_Task(void *parameter)
{
    void                   *pclient = NULL;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;

    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

    EXAMPLE_TRACE("mqtt example");

    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
    }

    while (1) {
        if (0 == loop_cnt % 20) {
            my_publish(pclient);
        }

        IOT_MQTT_Yield(pclient, 200);

        loop_cnt += 1;
    }
}

