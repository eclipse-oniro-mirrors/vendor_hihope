/*
 * Copyright (c) 2021 WinnerMicro Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ble_server_demo.h"

#include <stdio.h>
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "my_stdbool.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt.h"

#define BLE_DEMO_TASK_STACK_SIZE   2048
#define BLE_DEMO_TASK_PRIO         25
#define MS_1000     1000

void *BLE_Task_Entry(const char *arg)
{
    int rc;
    (void)arg;

    printf("Initialize BLE stack and running server demo.\r\n");

    /**Enable BLE stack*/
    /* 启用BLE堆栈 */
    rc = EnableBtStack();
    if (rc != OHOS_BT_STATUS_SUCCESS) {
        printf("ERR: enable ble stack failed, rc=%d\r\n", rc);
        return NULL;
    }

    /**Add BLE service and start Advertising*/
    /* 添加BLE服务并开始广告 */
    ble_server_demo();

    while (1) {
        osDelay(MS_1000); /* 延时1000ms */
    }
}

void BLE_Demo(void)
{
    osThreadAttr_t attr;

    attr.name       = "BLE_Demo_Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BLE_DEMO_TASK_STACK_SIZE; /* 堆栈大小2048 */
    attr.priority   = BLE_DEMO_TASK_PRIO;

    /* 创建线程监控是否创建蓝牙连接的任务 */
    if (osThreadNew((osThreadFunc_t)BLE_Task_Entry, NULL, &attr) == NULL) {
        printf("[BLEDemo] Failed to create BLE Demo Task!\n");
    }
}

SYS_RUN(BLE_Demo);
