 /*
 Copyright (C) 2024 HiHope Open Source Organization .
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

static int g_test_value = 0;

void number_thread(void *arg)
{
    osMutexId_t *mid = (osMutexId_t *)arg;
    const uint32_t DelayTimeMs = 100;
    const uint32_t remainder = 2;
    const uint32_t DelayTimeMs2 = 5;

    while (1) {
        if (osMutexAcquire(*mid, DelayTimeMs) == osOK) {
            g_test_value++;
            if (g_test_value % remainder == 0) {
                printf("[Mutex Test]%s gets an even value %d.\r\n", osThreadGetName(osThreadGetId()), g_test_value);
            } else {
                printf("[Mutex Test]%s gets an odd value %d.\r\n",  osThreadGetName(osThreadGetId()), g_test_value);
            }
            osMutexRelease(*mid);
            osDelay(DelayTimeMs2);
        }
    }
}

osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg)
{
    osThreadAttr_t attr = {
        name, 0, NULL, 0, NULL, 1024*2, osPriorityNormal, 0, 0
    };
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL) {
        printf("[Mutex Test]osThreadNew(%s) failed.\r\n", name);
    } else {
        printf("[Mutex Test]osThreadNew(%s) success, thread id: %d.\r\n", name, tid);
    }
    return tid;
}

void rtosv2_mutex_main(void *arg)
{
    (void)arg;
    osMutexAttr_t attr = {0};
    const uint32_t DelayTimeMs = 17;

    osMutexId_t mid = osMutexNew(&attr);
    if (mid == NULL) {
        printf("[Mutex Test]osMutexNew, create mutex failed.\r\n");
    } else {
        printf("[Mutex Test]osMutexNew, create mutex success.\r\n");
    }

    osThreadId_t tid1 = newThread("Thread_1", number_thread, &mid);
    osThreadId_t tid2 = newThread("Thread_2", number_thread, &mid);
    osThreadId_t tid3 = newThread("Thread_3", number_thread, &mid);

    osDelay(DelayTimeMs);
    osThreadId_t tid = osMutexGetOwner(mid);
    printf("[Mutex Test]osMutexGetOwner, thread id: %p, thread name: %s.\r\n", tid, osThreadGetName(tid));
    osDelay(DelayTimeMs);

    osThreadTerminate(tid1);
    osThreadTerminate(tid2);
    osThreadTerminate(tid3);
    osMutexDelete(mid);
}

static void MutexTestTask(void)
{
    osThreadAttr_t attr;

    attr.name = "rtosv2_mutex_main";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x1000;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)rtosv2_mutex_main, NULL, &attr) == NULL) {
        printf("[MutexTestTask] Falied to create rtosv2_mutex_main!\n");
    }
}

APP_FEATURE_INIT(MutexTestTask);