# 润和星闪派物联网开发套件--ADC（光敏电阻）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[润和星闪派物联网开发套件](https://item.taobao.com/item.htm?abbucket=16&id=816685710481&ns=1&priceTId=214783b117346662457694855ed644&skuId=5533042544092&spm=a21n57.sem.item.49.46a639031zWytE&utparam=%7B%22aplus_abtest%22%3A%22b28048df8f009463834be6bdac2a3713%22%7D&xxc=taobaoSearch) 基于海思WS63E解决方案的一套软硬件组合的综合性开发套件。

![wifi_iot](https://img.alicdn.com/imgextra/i4/3583112207/O1CN01SvRG981SAr7bdEg3i_!!3583112207.png)



## 一、OLED与主控芯片引脚的对应关系

炫彩板光敏电阻对应核心板GPIO9(ADC2)

## 二、如何编译

1. 将13_adclight目录复制到openharmony源码的`applications\sample\wifi-iot\app`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        ...
        "13_adclight:adc_light_demo",
        ...
    ]
```
3. 在`device\soc\hisilicon\ws63v100\sdk\build\config\target_config\ws63\config.py`文件中，找到`'ws63-liteos-app'`部分，在其`'ram_component'`中，添加以下代码：
```
"adc_light_demo"
```

4. 在`device\soc\hisilicon\ws63v100\sdk\libs_url\ws63\cmake\ohos.cmake`文件中，找到`"ws63-liteos-app"`部分，在其`set(COMPONENT_LIST`部分，添加以下代码：
```
"adc_light_demo"
```
5. 在openharmony sdk根目录目录执行：`rm -rf out && hb set -p nearlink_dk_3863 && hb build -f`

6. 如果编译失败，可以用串口助手，波特率设置为115200，发送AT指令(AT+SYSINFO\n)给开发板，如果返回的sdk版本低于SDK Version:1.10.102，需要替换一下adc相关的驱动文件，具体步骤参考adc_driver目录下替换文件步骤.txt

## 三、运行结果

烧录文件后，按下reset按键，程序开始运行，用手遮挡完全遮挡炫彩灯板上的光敏电阻，串口会打印一段时间的高电平数值，移开遮挡，串口会打印低电平数值。



### 【套件支持】

##### 1. 套件购买  https://item.taobao.com/item.htm?abbucket=16&id=816685710481&ns=1&priceTId=214783b117346662457694855ed644&skuId=5533042544092&spm=a21n57.sem.item.49.46a639031zWytE&utparam=%7B%22aplus_abtest%22%3A%22b28048df8f009463834be6bdac2a3713%22%7D&xxc=taobaoSearch

##### 2. 技术资料

- Gitee码云网站（使用说明书、规格说明书、OpenHarmony开发案例等） **https://gitee.com/hihopeorg_group/near-link**
- fbb_ws63代码仓（SDK包、技术文档下载）**https://gitee.com/HiSpark/fbb_ws63**

##### 3. 互动交流
- 海思社区星闪专区-论坛 **https://developer.hisilicon.com/forum/0133146886267870001**

