/**
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
 * limitations under the License.  *
 * Description: Provides adc port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "chip_core_irq.h"
#include "common_def.h"
#include "lpm_dev_ops.h"
#include "soc_osal.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "debug_print.h"
#include "hal_adc_v154.h"
#include "efuse_porting.h"
#include "adc_porting.h"

#define ADC_BASE_ADDR 0x4400C000
#define CLDO_BASE_ADDR 0X44001138

static uintptr_t g_adc_base_addr =  (uintptr_t)ADC_BASE_ADDR;
static uintptr_t g_cldo_cfg_addr =  (uintptr_t)CLDO_BASE_ADDR;

uint16_t g_voltage = 0;

uintptr_t adc_porting_base_addr_get(void)
{
    return g_adc_base_addr;
}

uintptr_t cldo_addr_get(void)
{
    return g_cldo_cfg_addr;
}

void adc_port_clock_enable(bool on)
{
    unused(on);
}

void adc_port_init_clock(adc_clock_t clock)
{
    unused(clock);
}

void adc_port_register_hal_funcs(void)
{
    hal_adc_register_funcs(hal_adc_v154_funcs_get());
}

static int32_t irq_adc_handler(int32_t irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    hal_adc_irq_handler();
    return 0;
}

void adc_port_register_irq(void)
{
    int ret = osal_irq_request(LSADC_IRQNR, (osal_irq_handler)irq_adc_handler, NULL, NULL, NULL);
    if (ret != 0) {
        print_str("adc_port_register_irq failed: %d\r\n", ret);
    } else {
        print_str("adc_port_register_irq succeed: %d\r\n", ret);
    }
    osal_irq_enable(LSADC_IRQNR);
}

void adc_port_unregister_hal_funcs(void)
{
    hal_adc_unregister_funcs();
}

void adc_port_unregister_irq(void)
{
    osal_irq_disable(LSADC_IRQNR);
    osal_irq_free(LSADC_IRQNR, NULL);
}

void adc_port_power_on(bool on)
{
    unused(on);
}

hal_adc_type_info_t *adc_port_get_cfg(void)
{
    return NULL;
}

errcode_t adc_port_get_cali_param(uint8_t *data_s, uint8_t *data_b, uint8_t *data_k)
{
    uint16_t data_len_s = 1, data_len_b = 2, data_len_k = 2;
    efuse_idx efuse_id = EFUSE_GET_S_ID;
    errcode_t res = efuse_read_item(efuse_id, data_s, data_len_s);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    efuse_id = EFUSE_GET_B_ID;
    res = efuse_read_item(efuse_id, data_b, data_len_b);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    efuse_id = EFUSE_GET_K_ID;
    res = efuse_read_item(efuse_id, data_k, data_len_k);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    return res;
}

static void adc_port_callback(uint8_t ch, uint32_t *buffer, uint32_t length, bool *next)
{
    unused(next);
    unused(ch);
    g_voltage = (uint16_t)buffer[length - 1];
}

errcode_t adc_port_read(uint8_t channel, uint16_t *data)
{
    uapi_adc_power_en(AFE_GADC_MODE, true);

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    errcode_t ret;
    adc_scan_config_t config = {0};
    ret = uapi_adc_auto_scan_ch_enable(channel, config, adc_port_callback);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_adc_auto_scan_ch_disable(channel);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    *data = g_voltage;
#endif
    uapi_adc_power_en(AFE_GADC_MODE, false);
    return ERRCODE_SUCC;
}