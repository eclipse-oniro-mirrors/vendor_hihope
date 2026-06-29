/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "at.h"
#include "osal_debug.h"

// 外部声明（来自主应用）
extern void OsDbgTskInfoGet(uint32_t task_mask);
#define OS_ALL_TASK_MASK  0xFFFFFFFF

// 内存统计相关外部声明（使用 extern，避免包含复杂头文件）
typedef struct {
    unsigned int uwTotalUsedSize;
    unsigned int uwTotalFreeSize;
    unsigned int uwMaxFreeNodeSize;
    unsigned int uwUsedNodeNum;
    unsigned int uwFreeNodeNum;
    unsigned int uwUsageWaterLine;
} LOS_MEM_POOL_STATUS;

extern unsigned int LOS_MemInfoGet(void *pool, LOS_MEM_POOL_STATUS *status);
extern unsigned char *m_aucSysMem0;

// PRINT 宏定义
#define PRINT osal_printk

// 内存使用打印函数
void print_mem_usage(const char *tag)
{
    LOS_MEM_POOL_STATUS status;
    LOS_MemInfoGet(m_aucSysMem0, &status);
    PRINT("[MEM Usage][%s] mem: used:%u, free:%u\r\n", tag, status.uwTotalUsedSize, status.uwTotalFreeSize);
}

// AT+SYSINFO 命令处理函数
static at_ret_t at_sys_info(void)
{
    osal_printk("[ZG]task_info:\r\n");
    print_mem_usage("AT");
    OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    return AT_RET_OK;
}

// AT命令表
static const at_cmd_entry_t at_custom_cmd_table[] = {
    {
        "SYSINFO2",
        100,
        0,
        NULL,
        at_sys_info,
        NULL,
        NULL,
        NULL,
    },
};

void at_custom_cmd_register(void)
{
    // 注册自定义AT命令
    uapi_at_cmd_table_register(at_custom_cmd_table, 
                               sizeof(at_custom_cmd_table) / sizeof(at_custom_cmd_table[0]),
                               0);
}
