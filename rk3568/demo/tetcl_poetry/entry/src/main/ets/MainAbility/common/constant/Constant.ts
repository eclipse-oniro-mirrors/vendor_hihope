/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */
/**
 * @Description 常量
 * @date 2022/7/18 12:15
 * @Author Bai XiaoMing
 * @Version 1.0
 */

export default class Constant {
    static readonly RequestApiKey: string = "你自己的APIKEY";

    static readonly BgColors: Array<any> = [
        [[0xFF6CAB, 0.0], [0x7366FF, 1.0]],
        [[0x64EBDE, 0.0], [0x8A64EB, 1.0]],
        [[0xFFCDA5, 0.0], [0xEE4D5F, 1.0]]
    ];

    static readonly PoetryFormat = ['诗句格式','五言诗', '七言诗'];

    static readonly PoetryType = ['藏头位置','藏头', '藏尾', '藏中', '递增', '递减'];

    static readonly PoetryPat = ['押韵类型','双句一压', '双句押韵', '一三四押'];
}