# 示例程序：led_rgb

 * Author: liangkz @ https://ost.51cto.com/column/46
 * Date  : 2022.04.01

## 1. 概述

- 当前已适配：**润和AI_Camera_Hi3516DV300开发板 、 润和DAYU200开发板**

- 软件分支：**OpenHarmony  Master(最新2022-07-01) or v3.1 Release(2022-03-30)** 

- 部署路径： **vendor/hihope/rk3568/demo/led_rgb/** 

- 注意：本示例程序以适配Master分支最新代码为例进行部署说明，对于LTS 3.0分支、3.1 Release分支或其他分支代码，请开发者自行修改一下代码、代码路径、编译脚本的相关配置即可。

本示例程序用于展示OpenHarmony设备驱动开发的相关要点，通过控制GPIO接口，点亮:

- Hi3516DV300开发板上的红色指示灯(R)、绿色指示灯(G)、红外补光灯(代码中标记为B)，在小型系统(适配LiteOS_A内核和Linux内核)、标准系统上都适用；
- DAYU200开发板上的RGB三色灯。
- 适配其他开发板，请自行添加 led_rgb/config/led/led_config_xxx.hcs配置文件，配置对应的LED灯的GPIO管脚编号。

## 2. 目录结构

```
./led_rgb/
├── apps
│   └── led_rgb
│       ├── BUILD.gn
│       └── led_rgb.c
├── config
│   ├── device_info
│   │   └── device_info.hcs
│   └── led
│       ├── led_config_hi3516.hcs
│       └── led_config_rk3568.hcs
├── drv
│   ├── build_linux
│   │   └── Makefile
│   ├── build_liteos
│   │   ├── BUILD.gn
│   │   └── Makefile
│   └── led_drv.c
└── README.md
```

## 3. 编译配置

### 3.1. apps/

烧录新编译的系统镜像到开发板上，在开发板的shell上执行：

```
./bin/led_rgb [args]
```

args = args&0x07，按位与取参数的低3位，分别对应B-G-R三色灯；对应位为1，点亮对应颜色的灯，对应位为0，熄灭对应颜色的灯。

不带 args 参数：默认依次执行7~0共计8组参数的灯控操作。

#### 3.1.1 小型系统（LiteOS_A内核和Linux内核）

在编译应用程序时，两个内核的小型系统的编译配置是一样的。

在最新Master分支代码中，已经没有//build/lite/components/applications.json 文件了，可以去修改 //applications/sample/camera/bundle.json文件，在"sub_component"下添加编译目标即可：

```
"//vendor/hihope/rk3568/demo/led_rgb/apps/led_rgb:led_rgb",
```



如果是LTS或Release分支的代码，则可以打开 //build/lite/components/applications.json 文件，找到camera_sample_app组件的信息，

在它的"dirs"和"targets"下，分别添加下面两句语句即可：

          "dirs": [
              "vendor/hihope/rk3568/demo/led_rgb/apps/led_rgb",
              ......
          ],
          "targets": [
              "//vendor/hihope/rk3568/demo/led_rgb/apps/led_rgb:led_rgb",
              ......
          ],
          "adapted_kernel": [ "liteos_a", "linux" ],

#### 3.1.2 标准系统（Linux内核）

在标准系统上配置应用的编译，与3.1.1的做法类似。

打开 //applications/standard/hap/ohos.build 文件，在其module_list上增加下面一句语句即可：

```
"//vendor/hihope/rk3568/demo/led_rgb/apps/led_rgb:led_rgb",
```



### 3.2. config/

这是led设备的hcs配置文件，按如下四部分进行配置，可分别将led设备的hcs配置文件加入到对应系统的设备配置树中，一起编译进内核并被使用。

#### 	3.2.1 小型系统（Hi3516开发板+LiteOS_A内核）

在//vendor/hisilicon/hispark_taurus/hdf_config/hdf.hcs文件的 include 部分增加两行代码即可：

    #include "../../../../vendor/hihope/rk3568/demo/led_rgb/config/led/led_config_hi3516.hcs"
    #include "../../../../vendor/hihope/rk3568/demo/led_rgb/config/device_info/device_info.hcs"

#### 	3.2.2 小型系统（Hi3516开发板+Linux内核）

在//vendor/hisilicon/hispark_taurus_linux/hdf_config/hdf.hcs文件的 include 部分增加两行代码即可：

    #include "../../../../vendor/hihope/rk3568/demo/led_rgb/config/led/led_config_hi3516.hcs"
    #include "../../../../vendor/hihope/rk3568/demo/led_rgb/config/device_info/device_info.hcs"

注意：修改了hdf.hcs文件，建议删除//vendor/hisilicon/hispark_taurus_linux/hdf_config/hdf_test/hdf_hcs.hcb文件后再重新编译，确保上述led的配置信息能编译进内核。

#### 	3.2.3 标准系统（Hi3516开发板+Linux内核）

在//vendor/hisilicon/Hi3516DV300/hdf_config/khdf/hdf.hcs文件的 include 部分增加两行代码即可：

    #include "../../../../../vendor/hihope/rk3568/demo/led_rgb/config/led/led_config_hi3516.hcs"
    #include "../../../../../vendor/hihope/rk3568/demo/led_rgb/config/device_info/device_info.hcs"

注意：修改了hdf.hcs文件，建议删除下列文件：

- //vendor/hisilicon/Hi3516DV300/hdf_config/khdf/hdf_test/hdf_hcs.hcb
- //out/KERNEL_OBJ/kernel/vendor/hisilicon/hispark_taurus_standard/hdf_config/khdf/hdf_test/hdf_hcs_hex.o
- //out/KERNEL_OBJ/kernel/OBJ/linux-5.10/arch/arm/boot/Image、uImage、zImage、zImage-dtb
- //out/hispark_taurus/packages/phone/images/uImage、zImage-dtb

再重新编译，确保上述led的配置信息能编译进内核。

#### 	3.2.4 标准系统（RK3568开发板+Linux内核）

​        在//vendor/hihope/rk3568/hdf_config/khdf/hdf.hcs文件的 include 部分增加两行代码即可：

    #include "../../../../../vendor/hihope/rk3568/demo/led_rgb/config/led/led_config_rk3568.hcs"
    #include "../../../../../vendor/hihope/rk3568/demo/led_rgb/config/device_info/device_info.hcs"

注意：修改了hdf.hcs文件，建议删除下列文件：

- //vendor/hihope/rk3568/hdf_config/khdf/hdf_test/hdf_hcs.hcb
- //out/kernel/vendor/hihope/rk3568/hdf_config/khdf/hdf_test/hdf_hcs_hex.o
- //out//kernel/OBJ/linux-5.10/arch/arm64/boot/Image、Image.lz4
- //out/rk3568/packages/phone/images/boot_linux.img

再重新编译，确保上述led的配置信息能编译进内核。



### 3.3. drv/

这里存放着驱动程序的实现代码，以及编译配置脚本。

驱动开发工程师根据项目的实际情况来修改这里的实现和编译配置。

#### 	3.3.1 drv/src/

这是驱动程序的实现代码存放路径，与内核无关。

#### 	3.3.2 drv/build_liteos_a/

这是驱动程序适配LiteOS_A内核的小型系统的编译配置文件。

**BUILD.gn**：将编译出来的驱动入口“g_ledDriverEntryHdfEntry”链接到OHOS_Image.bin中，有模板的。

这个是BUILD.gn脚本所在路径。BUILD.gn中module_switch用不到就要注释掉，否则会导致驱动入口 “g_ledDriverEntryHdfEntry”无法链接到OHOS_Image.bin中。

在//device/soc/hisilicon/common/platform/BUILD.gn文件的deps关系中，添加如下一句语句:

```
"//vendor/hihope/rk3568/demo/led_rgb/drv/build_liteos"
```

#### 	3.3.3 drv/build_linux/

这是驱动程序适配Linux内核的小型系统和标准系统的编译配置文件。

**Makefile**：编译drv/src/目录下源代码的脚本，有模板的。

在//drivers/hdf_core/adapter/khdf/linux/Makefile文件的末尾，添加如下一句语句即可：

```
obj-$(CONFIG_DRIVERS_HDF) += ../../../../../vendor/hihope/rk3568/demo/led_rgb/drv/build_linux/
```

把这个Makefile纳入khdf/linux的编译体系中。
