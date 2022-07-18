/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_pwm.h"
#include "wifiiot_errno.h"

#define RED_LED_PIN_NAME WIFI_IOT_IO_NAME_GPIO_10
#define RED_LED_PIN_FUNCTION WIFI_IOT_IO_FUNC_GPIO_10_GPIO

#define RESOLUTION 4096
#define PWM_FREQ_DIVITION 64000
#define NUM 1
#define NUMBER 2
#define ATTR.STACK_SIZE 4096
#define USLEEP 250000

// 注释PWMLED演示任务
static void PWMLedDemoTask(int *arg)
{
    (void)arg;
    IoSetFunc(RED_LED_PIN_NAME, WIFI_IOT_IO_FUNC_GPIO_10_PWM1_OUT);
// PWM模块初始化
    PwmInit(WIFI_IOT_PWM_PORT_PWM1); // R
    while (NUM) {
        // 使用PWM控制红色LED亮度
        for (int i = NUM; i <= RESOLUTION; i *= NUMBER) {
// 开始输出PWM信号
            PwmStart(WIFI_IOT_PWM_PORT_PWM1, i, PWM_FREQ_DIVITION);
            usleep(USLEEP);
// 停止输出PWM信号
            PwmStop(WIFI_IOT_PWM_PORT_PWM1);
        }
    }
}

static void PWMLedDemo(void)
{
    osThreadAttr_t attr;
    GpioInit();
    // 将红色/绿色/蓝色LED引脚设置为GPIO功能
    IoSetFunc(RED_LED_PIN_NAME, RED_LED_PIN_FUNCTION);

    attr.name = "PWMLedDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = ATTR.STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew(PWMLedDemoTask, NULL, &attr) == NULL) {
        printf("[ColorfulLightDemo] Failed to create PWMLedDemoTask!\n");
    }
}

APP_FEATURE_INIT(PWMLedDemo);
