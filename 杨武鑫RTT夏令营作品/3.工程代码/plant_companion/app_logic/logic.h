/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-01     ywx       the first version
 */
#ifndef APPLICATIONS_APP_LOGIC_LOGIC_H_
#define APPLICATIONS_APP_LOGIC_LOGIC_H_

#define AHT10_I2C_BUS "i2c2"

#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     12           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

void access_Sensor(void);
void Sensor_Logic_Running(void);

#endif /* APPLICATIONS_APP_LOGIC_LOGIC_H_ */
