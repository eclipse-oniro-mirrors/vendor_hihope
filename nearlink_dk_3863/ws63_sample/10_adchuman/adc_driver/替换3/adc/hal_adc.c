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

#include "common_def.h"
#include "hal_adc.h"

hal_adc_funcs_t *g_hal_adc_funcs = NULL;

errcode_t hal_adc_register_funcs(hal_adc_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_adc_funcs = funcs;

    return ERRCODE_SUCC;
}

errcode_t hal_adc_unregister_funcs(void)
{
    g_hal_adc_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_adc_funcs_t *hal_adc_get_funcs(void)
{
    return g_hal_adc_funcs;
}