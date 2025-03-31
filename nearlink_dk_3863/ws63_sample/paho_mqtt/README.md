# OpenHarmony兼容版paho.mqtt.embedded-c模块

## 简介

1. 支持OpenHarmony liteOS-M内核，包括Hi3861、Hi3863开发板。
2. 支持OpenHarmony liteOS-A内核，包括Hi3516, Hi3518开发板。
3. 测试程序支持Linux平台。

## 编译

1. 修改`applications\sample\wifi-iot\app\BUILD.gn`文件，放行以下代码：
```gn
# Application "paho_mqtt"
lite_component("app") {
  features = [
    "paho_mqtt/MQTTPacket/src:paho-embed-mqtt3c",          # MQTTPacket模块
    "paho_mqtt/MQTTClient-C/src:paho-embed-mqtt3cc",       # MQTTClient-C模块
  ]
}
```

2. 在`device\soc\hisilicon\ws63v100\sdkv100\build\config\target_config\ws63\config.py`文件中，找到`'ws63-liteos-app'`部分，在其`'ram_component'`中，放行以下代码：
```python
"paho-embed-mqtt3c", "paho-embed-mqtt3cc"
```

3. 在`device\soc\hisilicon\ws63v100\sdkv100\libs_url\ws63\cmake\ohos.cmake`文件中，找到`set(COMPONENT_LIST`部分，放行以下代码：
```cmake
"paho-embed-mqtt3c" "paho-embed-mqtt3cc"
```

4. 在OpenHarmony SDK根目录（例如`~/openharmony/sdk20240628`）下，执行如下命令进行编译：
```shell
rm -rf out && hb set -p nearlink_dk_3863 && hb build -f
```

5. 编译日志中出现如下信息，表示编译成功：
```verilog
[OHOS INFO] nearlink_dk_3863 build success
[OHOS INFO] Cost time:  0:00:18
```
- 编译成功后，生成的固件文件在`out\nearlink_dk_3863\nearlink_dk_3863\ws63-liteos-app`目录下，文件名为`ws63-liteos-app_load_only.fwpkg`，或者`ws63-liteos-app_all.fwpkg`。
- 如果编译失败，根据编译日志中的错误信息，结合`out\nearlink_dk_3863\nearlink_dk_3863`目录中的编译日志文件（*.log）中的错误信息，定位错误，修改代码后重新编译。

## 烧录

使用"BurnTool"或者"快速烧录工具"，将编译生成的`ws63-liteos-app_load_only.fwpkg`或者`ws63-liteos-app_all.fwpkg`固件文件烧录到开发板中。

## 硬件准备
- 核心板

## 运行

运行串口工具（SecureCRT、MobaXterm等），并连接到开发板串口。重启开发板，查看串口输出信息。

## 运行效果

基础模块，无实质性运行效果。
