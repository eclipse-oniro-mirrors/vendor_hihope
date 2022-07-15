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
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "gpio_if.h"

#define HDF_LOG_TAG led_drv
#define LED_RGB_WRITE  7
#define LED_R_BIT      0x01
#define LED_G_BIT      0x02
#define LED_B_BIT      0x04

#define LED_VERSION    2
#define LED_MODE_OFF   0
#define LED_MODE_ON    1
#define LED_MODE_FLIP  (-1)

struct led_config {
    uint32_t led_ver;
    uint32_t led_R;
    uint32_t led_G;
    uint32_t led_B;
};
struct led_config g_ledcfg = {0};

static int32_t LedGpioCtrl(uint16_t gpio, int32_t mode)
{
    uint16_t level = GPIO_VAL_HIGH;

    if (HDF_SUCCESS != GpioSetDir(gpio, GPIO_DIR_OUT)) {
        HDF_LOGE("LedGpioCtrl: GpioSetDir NG");
        return HDF_FAILURE;
    }

    if (mode == LED_MODE_ON) {
        level = GPIO_VAL_HIGH;
    } else if (mode == LED_MODE_OFF) {
        level = GPIO_VAL_LOW;
    } else {  /* default to LED_MODE_FLIP */
        GpioRead(gpio, &level);
        level = (level+1)&0x01;
    }

    if (HDF_SUCCESS != GpioWrite(gpio, level)) {
        HDF_LOGE("LedGpioCtrl: GpioWrite NG");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t LedDriverDispatch(struct HdfDeviceIoClient *client, int32_t cmdId,  \
                          struct HdfSBuf *dataBuf, struct HdfSBuf *replyBuf)
{
    int32_t result = HDF_FAILURE;
    int32_t LedMode = LED_MODE_FLIP;

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("Led driver device is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (g_ledcfg.led_ver != LED_VERSION) {
        HDF_LOGE("Led driver g_ledcfg.led_ver NOT match");
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case LED_RGB_WRITE:
            result = HdfSbufReadInt32(dataBuf, &LedMode);
            if (result) {
                HDF_LOGI("LedDriverDispatch/RGB: LedMode[0x%X][%s%s%s]",
                         LedMode, (LedMode&LED_R_BIT)?"R":"-",
                         (LedMode&LED_G_BIT)?"G":"-", (LedMode&LED_B_BIT)?"B":"-");
                LedGpioCtrl(g_ledcfg.led_R, (LedMode&LED_R_BIT)?1:0);
                LedGpioCtrl(g_ledcfg.led_G, (LedMode&LED_G_BIT)?1:0);
                LedGpioCtrl(g_ledcfg.led_B, (LedMode&LED_B_BIT)?1:0);
            }
            break;
        default:
            HDF_LOGE("LedDriverDispatch: receiving unknown cmdId, NG!!");
            break;
    }
    return result;
}

int32_t HdfLedDriverBind(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        HDF_LOGE("HDF_INIT(g_ledDriverEntry): HdfLedDriverBind: NG!");
        return HDF_ERR_INVALID_OBJECT;
    }
    static struct IDeviceIoService ledDriverServ = {
        .Dispatch = LedDriverDispatch,
    };
    deviceObject->service = (struct IDeviceIoService *)(&ledDriverServ);
    HDF_LOGI("HDF_INIT(g_ledDriverEntry): HdfLedDriverBind: OK! NodeName[%s]",
             deviceObject->property->name);
    return HDF_SUCCESS;
}

int32_t HdfLedDriverInit(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        HDF_LOGE("HDF_INIT(g_ledDriverEntry): HdfLedDriverInit: NG!");
        return HDF_ERR_INVALID_OBJECT;
    }
    struct DeviceResourceIface *CfgOps = NULL;
    CfgOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (CfgOps == NULL || CfgOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid CfgOps fail!", __func__);
        return HDF_FAILURE;
    }
    if (CfgOps->GetUint32(deviceObject->property, "led_version", &g_ledcfg.led_ver, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read led_version fail!", __func__);
        return HDF_FAILURE;
    }
    if (CfgOps->GetUint32(deviceObject->property, "led_R", &g_ledcfg.led_R, 0) != HDF_SUCCESS) {
        g_ledcfg.led_ver = 0;
        HDF_LOGE("%s: read led_R fail!", __func__);
        return HDF_FAILURE;
    }
    if (CfgOps->GetUint32(deviceObject->property, "led_G", &g_ledcfg.led_G, 0) != HDF_SUCCESS) {
        g_ledcfg.led_ver = 0;
        HDF_LOGE("%s: read led_G fail!", __func__);
        return HDF_FAILURE;
    }
    if (CfgOps->GetUint32(deviceObject->property, "led_B", &g_ledcfg.led_B, 0) != HDF_SUCCESS) {
        g_ledcfg.led_ver = 0;
        HDF_LOGE("%s: read led_B fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("HDF_INIT(g_ledDriverEntry): HdfLedDriverInit: OK!");
    return HDF_SUCCESS;
}

void HdfLedDriverRelease(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        HDF_LOGE("HDF_INIT(g_ledDriverEntry): HdfLedDriverRelease: NG!");
        return;
    }
    HDF_LOGI("HDF_INIT(g_ledDriverEntry): HdfLedDriverRelease: OK!");
    return;
}

struct HdfDriverEntry g_ledDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "led_driver",
    .Bind = HdfLedDriverBind,
    .Init = HdfLedDriverInit,
    .Release = HdfLedDriverRelease,
};
HDF_INIT(g_ledDriverEntry);