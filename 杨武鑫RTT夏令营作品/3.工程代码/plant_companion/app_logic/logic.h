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

#define ADC_DEV_NAME        "adc1"      /* ADC �豸���� */
#define ADC_DEV_CHANNEL     12           /* ADC ͨ�� */
#define REFER_VOLTAGE       330         /* �ο���ѹ 3.3V,���ݾ��ȳ���100����2λС��*/
#define CONVERT_BITS        (1 << 12)   /* ת��λ��Ϊ12λ */

void access_Sensor(void);
void Sensor_Logic_Running(void);

#endif /* APPLICATIONS_APP_LOGIC_LOGIC_H_ */
