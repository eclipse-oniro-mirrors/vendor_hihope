/*
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*/
#include "common_def.h"
#include "iot_errno.h"
#include "pwm.h"
#include "iot_pwm.h"

#define IOT_PWM_MIN_DUTY           0
#define IOT_PWM_MAX_DUTY           100
#define IOT_PWM_DUTY_PART          10
#define IOT_PWM_FREQ_PART          2
#define IOT_PWM_MAX_DIV_NUM_MAX    8
#define IOT_PWM_CFG_OFFEST_TIME    0
#define IOT_PWM_CFG_REPEAT_CYCLE   0
#define IOT_PWM_CFG_REPEAT_STATE   true

static bool g_iot_pwm_inited = false;

unsigned int IoTPwmInit(unsigned int port)
{
    unused(port);
    if (!g_iot_pwm_inited) {
        uapi_pwm_init();
    }
    g_iot_pwm_inited = true;
    return IOT_SUCCESS;
}

unsigned int IoTPwmDeinit(unsigned int port)
{
    unused(port);
    if (g_iot_pwm_inited) {
        uapi_pwm_deinit();
    }
    g_iot_pwm_inited = false;
    return IOT_SUCCESS;
}

unsigned int IoTPwmStart(unsigned int port, unsigned short duty, unsigned int freq)
{
    
    if ((duty > IOT_PWM_MAX_DUTY) || (duty < IOT_PWM_MIN_DUTY)) {
        return IOT_FAILURE;
    }

    uint32_t clk_freq = uapi_pwm_get_frequency(0);
    uint32_t period = 0;
    if(freq != 0){
        period = (uint32_t)(2*clk_freq/freq);
    }
    uint32_t high_time  = (uint32_t)(period*duty/100);
    uint32_t low_time = (uint32_t)(period- high_time); 
    uint8_t group_id = 0;
    pwm_config_t cfg = {
                            low_time,
                            high_time, // 高电平持续tick 时间 = tick * (1/32000000)
                            0,     // 相位偏移位
                            0,     // 发多少个波形
                            true   // 是否循环
                        }; 
    //关闭 PWM
    uapi_pwm_close(port);
    if (uapi_pwm_open((uint8_t)port, &cfg) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
     
    switch (port)
    {
        case 0:
        case 1:
            group_id = 0;
            break;
        case 2:
        case 3:
            group_id = 1;
            break;
        case 4:
        case 5: 
            group_id = 2;
            break;
        case 6: 
        case 7: 
            group_id = 3;
            break;   
        default:
            break;
    }
    
   
    uapi_pwm_clear_group(group_id);
    if (uapi_pwm_set_group(group_id, &port,1) != ERRCODE_SUCC) {  // 每个通道对应一个bit位
        return IOT_FAILURE;
    }

    uapi_pwm_start(port);
    return IOT_SUCCESS;
}

unsigned int IoTPwmStop(unsigned int port)
{

    return IoTPwmStart(port,0,2000);
}
