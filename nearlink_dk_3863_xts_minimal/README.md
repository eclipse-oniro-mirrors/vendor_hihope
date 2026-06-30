# nearlink_dk_3863_xts_minimal 产品配置说明

## 📁 目录结构

```
vendor/hihope/nearlink_dk_3863_xts_minimal/
├── BUILD.gn                    # GN 构建配置（含 copy action）
├── ohos.build                  # OpenHarmony 部件配置
├── config.json                 # 产品配置文件
├── README.md                   # 📖 使用说明文档
├── wraps/                      # 弱引用实现目录
│   ├── BUILD.gn               # wraps 库构建配置
│   ├── stub_impl.c            # ROM 组件裁剪的弱引用实现
│   └── at_custom_cmd.c        # 🔧 自定义 AT 命令实现
└── config_patch.py             # 🔧 SDK 配置补丁文件（自动拷贝到 SDK）
```

## 🔄 自动化流程

### 1. GN 构建阶段

```bash
# 在 OpenHarmony 根目录执行
python3 build.py --product-name nearlink_dk_3863_xts_minimal  --gn-args build_xts=true  --test xts //test/xts/acts/startup_lite/bootstrap_hal:ActsBootstrapTest,//test/xts/acts/distributed_schedule_lite/system_ability_manager_hal:ActsSamgrTest,//test/xts/acts/hiviewdfx_lite/hievent_hal:ActsHieventLiteTest,//test/xts/acts/startup_lite/syspara_hal:ActsParameterTest,//test/xts/acts/hiviewdfx_lite/hilog_hal:ActsDfxFuncTest
```

## 📡 自定义 AT 命令

### 已实现的 AT 命令

**AT+SYSINFO** - 系统信息查询
- **功能**：打印任务信息、内存使用情况
- **实现位置**：`wraps/at_custom_cmd.c`
- **使用示例**：
  ```
  AT+SYSINFO?
  [ZG]task_info:
  [MEM Usage][AT] mem: used:12345, free:67890
  TaskName        State   Priority  StackSize  UsedStack
  ...
  OK
  ```

### 添加新的 AT 命令

编辑 `wraps/at_custom_cmd.c`，添加新的命令处理函数和表项：

```c
// 1. 实现命令处理函数
static at_ret_t at_your_command(void)
{
    osal_printk("Your command handler\r\n");
    return AT_RET_OK;
}

// 2. 在 at_custom_cmd_table 中添加新条目
static const at_cmd_entry_t at_custom_cmd_table[] = {
    {
        "SYSINFO",      // 已有命令
        100,
        0,
        NULL,
        at_sys_info,
        NULL,
        NULL,
        NULL,
    },
    {
        "YOURCMD",      // 新命令
        100,
        0,
        NULL,
        at_your_command,  // 处理函数
        NULL,
        NULL,
        NULL,
    },
};
```

重新构建后，新的 AT 命令会自动注册。

## 📝 config_patch.py 功能

### 主要修改项

1. **禁用 HSO 日志和蓝牙**
   ```python
   target["ws63-liteos-xts"]["hso_enable_bt"] = False
   target["ws63-liteos-xts"]["hso_enable"] = False
   ```

2. **裁剪 RAM 组件**
   ```python
   target["ws63-liteos-xts"]["ram_component_set"] = [
       'i2c', 'uart', 'hso_log', 
       '-:security_unified',  # 删除安全驱动
       'dfx_set', 'sio_v151', 'i2s'
   ]
   ```

3. **裁剪 ROM 组件**
   ```python
   target["ws63-liteos-xts"]['rom_component'] = [
       '-:bt_host_rom',      # 删除蓝牙 ROM 组件
   ]
   ```

4. **跳过 ROM Patch 生成**
   ```python
   target["ws63-liteos-xts"]['skip_rom_patch'] = True
   ```

5. **动态修改 Kconfig**
   - 禁用 FTM、NV、UPG 等中间件
   - 调整 LiteOS 默认栈大小为 1024

## 🔧 自定义配置

如果需要修改配置，编辑 `config_patch.py` 中的 `patch_target()` 函数：

```python
def patch_target(target):
    # 修改目标配置
    target["ws63-liteos-xts"]["defines"].append("YOUR_DEFINE")
    target["ws63-liteos-xts"]["ram_component"].append("your_component")
    
    # 修改 Kconfig
    kconfig_changes['YOUR_KCONFIG_OPTION'] = 'y'
```

修改后重新运行 `copy_to_sdk.py` 即可生效。

## ⚠️ 注意事项

1. **自动拷贝**：GN 构建会自动拷贝 `config_patch.py`，无需手动操作
2. **路径依赖**：BUILD.gn 中的拷贝路径假设 SDK 位于标准位置
3. **配置冲突**：如果 `config_patch.py` 和 SDK 原有配置冲突，以最后应用的为准
4. **Kconfig 修改**：Kconfig 修改采用“注释旧行+添加新行”策略，保留历史记录

## 🐛 故障排查

### 问题 1：配置文件未拷贝到 SDK

检查 BUILD.gn 中的 copy action 是否正确配置：
```gn
copy("copy_config_to_sdk") {
    sources = [ "config_patch.py" ]
    outputs = [ 
        "$root_out_dir/../device/soc/hisilicon/ws63v100/sdk/build/config/target_config/ws63/config_patch_nearlink.py"
    ]
}
```

### 问题 2：配置补丁未生效

检查 SDK config.py 是否有导入语句：
```python
import config_patch_nearlink
config_patch_nearlink.patch_target(target)
```

### 问题 3：Kconfig 修改失败

确认 Kconfig 文件路径正确：
```python
kconfig_xts = os.path.join(..., 'ws63_liteos_xts.config')
print(f"Checking: {kconfig_xts}")  # 添加调试输出
```

## 📚 相关文档

- [OpenHarmony GN 构建系统](https://gitee.com/openharmony/docs)
- [WS63 SDK 开发指南](device/soc/hisilicon/ws63v100/sdk/README.md)
- [Kconfig 配置说明](kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/build/menuconfig/README.md)
