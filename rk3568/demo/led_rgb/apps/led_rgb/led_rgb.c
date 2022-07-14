/* Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
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
 *
 * Author: liangkz @ https://ost.51cto.com/column/46
 * Date  : 2022.04.01
 *
 */
#include "unistd.h"
#include "stdlib.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hilog/log.h"

#undef  LOG_TAG
#undef  LOG_DOMAIN
#define LOG_TAG    "led_rgb"
#define LOG_DOMAIN 0xD002022
#define LED_SERVICE_NAME "led_service"

#define ARGS_NUM       2
#define LED_SLEEP_TIME 2
#define LED_RGB_WRITE  7
#define LED_R_BIT      0x01
#define LED_G_BIT      0x02
#define LED_B_BIT      0x04

int main(int argc, char* argv[])
{
    int ret = HDF_SUCCESS;
    int32_t led_mod = -1;
    if (argc >= ARGS_NUM) {
        led_mod = atoi(argv[1]);
        led_mod &= 0x07;  /* low-3bits */
        HILOG_INFO(LOG_APP, "[%{public}s] main enter: led_mod[%{public}s%{public}s%{public}s][0x%{public}X]",
                   LOG_TAG, (led_mod&LED_B_BIT)?"B":"-", (led_mod&LED_G_BIT)?"G":"-",
                   (led_mod&LED_R_BIT)?"R":"-", led_mod);
    } else {
        HILOG_INFO(LOG_APP, "[%{public}s] main enter: Auto Test", LOG_TAG);
    }

    struct HdfIoService *serv = HdfIoServiceBind(LED_SERVICE_NAME);
    if (serv == NULL) {
        HILOG_ERROR(LOG_APP, "get service %{public}s NG", LED_SERVICE_NAME);
        return -1;
    }
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HILOG_ERROR(LOG_APP, "obtain data NG");
        return -1;
    }

    if (led_mod == -1) {
        led_mod = 0x08;
        while (led_mod) {
            HdfSbufFlush(data);
            if (!HdfSbufWriteInt32(data, --led_mod)) {
                HILOG_ERROR(LOG_APP, "write data NG");
                return -1;
            }
            ret = serv->dispatcher->Dispatch(&serv->object, LED_RGB_WRITE, data, NULL);
            sleep(LED_SLEEP_TIME);
        }
    } else {
        if (!HdfSbufWriteInt32(data, led_mod)) {
            HILOG_ERROR(LOG_APP, "write data NG");
            return -1;
        }
        ret = serv->dispatcher->Dispatch(&serv->object, LED_RGB_WRITE, data, NULL);
    }

    HdfSbufRecycle(data);
    HdfIoServiceRecycle(serv);
    HILOG_INFO(LOG_APP, "[%{public}s] main exit.", LOG_TAG);

    return ret;
}