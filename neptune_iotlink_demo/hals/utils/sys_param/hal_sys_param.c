/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_sys_param.h"
#include "wm_internal_flash.h"

static const char OHOS_DEVICE_TYPE[] = {"Development Board"};
static const char OHOS_DISPLAY_VERSION[] = {"OpenHarmony-master"};
static const char OHOS_MANUFACTURE[] = {"winnermicro"};
static const char OHOS_BRAND[] = {"hihope"};
static const char OHOS_MARKET_NAME[] = {"****"};
static const char OHOS_PRODUCT_SERIES[] = {"****"};
static const char OHOS_PRODUCT_MODEL[] = {"****"};
static const char OHOS_SOFTWARE_MODEL[] = {"****"};
static const char OHOS_HARDWARE_MODEL[] = {"****"};
static const char OHOS_HARDWARE_PROFILE[] = {"****"};
static const char OHOS_BOOTLOADER_VERSION[] = {"bootloader"};
static const char OHOS_ABI_LIST[] = {"****"};
static const char OHOS_SERIAL[] = {"1234567890"};
static const int OHOS_FIRST_API_VERSION = 1;

static const char EMPTY_STR[] = {""};

#define HEX_A 0xa
#define CHAR_NUM_OFFSET 0x30
#define CHAR_CAPITAL_OFFSET 0x37

#define FLASH_UNI_ID_LEN   18
#define SERIAL_NUM_LEN   (FLASH_UNI_ID_LEN*2+1)
#define STR_END_FLAG '\0'

static char serialNumber[SERIAL_NUM_LEN] = {0};

/* 十六进制数转换为字符类型数据 */
static char Hex2Char(u8 hex)
{
    if (hex < HEX_A) {
        return hex + CHAR_NUM_OFFSET;
    } else {
        return hex + CHAR_CAPITAL_OFFSET;
    }
}

/* 获取设备类型 */
const char* HalGetDeviceType(void)
{
    return OHOS_DEVICE_TYPE;
}

/* 获取厂商信息 */
const char* HalGetManufacture(void)
{
    return OHOS_MANUFACTURE;
}

/* 获取商标名称 */
const char* HalGetBrand(void)
{
    return OHOS_BRAND;
}

const char* HalGetMarketName(void)
{
    return OHOS_MARKET_NAME;
}

/* 获取产品系列名称 */
const char* HalGetProductSeries(void)
{
    return OHOS_PRODUCT_SERIES;
}

/* 获取产品型号 */
const char* HalGetProductModel(void)
{
    return OHOS_PRODUCT_MODEL;
}

const char* HalGetSoftwareModel(void)
{
    return OHOS_SOFTWARE_MODEL;
}

const char* HalGetHardwareModel(void)
{
    return OHOS_HARDWARE_MODEL;
}

const char* HalGetHardwareProfile(void)
{
    return OHOS_HARDWARE_PROFILE;
}

const char* HalGetSerial(void)
{
    const char half_char = 4;
    char flashUniqueId[FLASH_UNI_ID_LEN];

    if (serialNumber[0] == STR_END_FLAG) {
        tls_fls_read_unique_id(flashUniqueId);
        int j = 0;
        for (int i = 0; i < FLASH_UNI_ID_LEN; i++) {
            u8 lowFour, highFour;
            highFour = (flashUniqueId[i] & 0xF0) >> half_char;
            serialNumber[j] = Hex2Char(highFour);
            j++;
            lowFour = flashUniqueId[i] & 0xF;
            serialNumber[j] = Hex2Char(lowFour);
            j++;
        }
    }
    return serialNumber;
}

const char* HalGetBootloaderVersion(void)
{
    return OHOS_BOOTLOADER_VERSION;
}

const char* HalGetAbiList(void)
{
    return OHOS_ABI_LIST;
}

const char* HalGetDisplayVersion(void)
{
    return OHOS_DISPLAY_VERSION;
}

const char* HalGetIncrementalVersion(void)
{
    return INCREMENTAL_VERSION;
}

const char* HalGetBuildType(void)
{
    return BUILD_TYPE;
}

const char* HalGetBuildUser(void)
{
    return BUILD_USER;
}

const char* HalGetBuildHost(void)
{
    return BUILD_HOST;
}

const char* HalGetBuildTime(void)
{
    return BUILD_TIME;
}

int HalGetFirstApiVersion(void)
{
    return OHOS_FIRST_API_VERSION;
}
