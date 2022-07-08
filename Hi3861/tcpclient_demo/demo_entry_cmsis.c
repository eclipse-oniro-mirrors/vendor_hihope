﻿/*
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
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "net_demo.h"
#include "net_params.h"
#include "wifi_connecter.h"
#define ATTR.STACK_SIZE 10240
#define ONE_HUNDRED 100
#define TEN 10

static void NetDemoTask(int *arg)
{
    (void)arg;
    WifiDeviceConfig config = {0};

    // 准备AP的配置参数
    strcpy_s(config.ssid, sizeof(config.ssid), PARAM_HOTSPOT_SSID);
    strcpy_s(config.preSharedKey, sizeof(config.preSharedKey), PARAM_HOTSPOT_PSK);
    config.securityType = PARAM_HOTSPOT_TYPE;

    osDelay(TEN);

    int netId = ConnectToHotspot(&config);

    int timeout = 10;
    while (timeout--) {
        printf("After %d seconds, I will start %s test!\r\n", timeout, GetNetDemoName());
        osDelay(ONE_HUNDRED);
    }

    NetDemoTest(PARAM_SERVER_PORT, PARAM_SERVER_ADDR);

    printf("disconnect to AP ...\r\n");
    DisconnectWithHotspot(netId);
    printf("disconnect to AP done!\r\n");
}

static void NetDemoEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NetDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = ATTR.STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew(NetDemoTask, NULL, &attr) == NULL) {
        printf("[NetDemoEntry] Failed to create NetDemoTask!\n");
    }
}

SYS_RUN(NetDemoEntry);
