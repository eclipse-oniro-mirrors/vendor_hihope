 /*
 Copyright (C) 2024 HiHope Open Source Organization .
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */


// AHT20数字温湿度传感器驱动接口文件
#ifndef AHT20_H
#define AHT20_H

#include <stdint.h>   

uint32_t AHT20_Calibrate(void);
uint32_t AHT20_StartMeasure(void);
uint32_t AHT20_GetMeasureResult(float* temp, float* humi);


#endif  
