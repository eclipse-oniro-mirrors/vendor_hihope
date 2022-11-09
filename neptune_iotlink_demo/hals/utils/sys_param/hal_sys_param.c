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
