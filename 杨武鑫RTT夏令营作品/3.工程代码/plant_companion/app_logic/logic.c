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

#include "logic.h"
#include "sensor.h"
#include "sensor_dallas_dht11.h"
#include "drv_gpio.h"

rt_adc_device_t adc_dev;

rt_uint32_t cur_hum, cur_tem;
extern rt_uint32_t hum, tem;

/* 邮箱控制块 */
extern struct rt_mailbox tem_mb;
extern struct rt_mailbox hum_mb;
/* 用于放邮件的内存池 */
extern char mb_hum_pool[128];
extern char mb_tem_pool[128];

#define LED0_PIN GET_PIN(D, 3)
#define LED1_PIN GET_PIN(D, 4)
#define LED2_PIN GET_PIN(D, 5)
#define LED3_PIN GET_PIN(D, 6)

//继电器
#define RELAY_PIN GET_PIN(C, 6)

#define DHT11_DATA_PIN GET_PIN(D, 1)

#define TEM_MAX 33
#define HUM_MIN 70

/*传感器控制逻辑*/
void Sensor_Logic_Running(void)
{

    rt_mb_recv(&hum_mb, (rt_uint32_t *)&cur_hum, RT_WAITING_FOREVER);
    rt_mb_recv(&tem_mb, (rt_uint32_t *)&cur_tem, RT_WAITING_FOREVER);


    if(cur_tem >= TEM_MAX)
    {
       rt_pin_write(LED0_PIN, PIN_HIGH);
    }
    else {
        rt_pin_write(LED0_PIN, PIN_LOW);
    }
    if(cur_hum <= HUM_MIN)
    {
       rt_kprintf("start watering\r\n");
       rt_pin_write(LED2_PIN, PIN_HIGH);
       rt_pin_write(RELAY_PIN, PIN_HIGH);
    }
    else {
        rt_pin_write(LED2_PIN, PIN_LOW);
        rt_pin_write(RELAY_PIN, PIN_LOW);
    }
}

/*访问传感器*/
void access_Sensor(void)
{
    //DHT11
    struct rt_sensor_config cfg;

    cfg.intf.user_data = (void *)DHT11_DATA_PIN;
    rt_hw_dht11_init("dht11", &cfg);
/*
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
    rt_size_t res;
    rt_uint8_t get_data_freq = 1;  1Hz

    dev = rt_device_find("temp_dht11");
    if (dev == RT_NULL)
    {
        return;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("open device failed!\n");
        return;
    }

    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));

    while (1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);

        if (res != 1)
        {
            rt_kprintf("read data failed! result is %d\n", res);
            rt_device_close(dev);
            return;
        }
        else
        {
            if (sensor_data.data.temp >= 0)
            {
                uint8_t temp = (sensor_data.data.temp & 0xffff) >> 0;      // get temp
                tem = (rt_uint32_t)temp;
                //uint8_t humi = (sensor_data.data.temp & 0xffff0000) >> 16; // get humi
            }
        }
    }*/

    //土壤湿度

    /* 查找设备 */
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
    }

    /* 使能设备 */
    rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);

/*  //读取采样值
    hum = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);

    //关闭通道
    ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);*/

}


