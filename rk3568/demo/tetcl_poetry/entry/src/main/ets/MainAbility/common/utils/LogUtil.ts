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
 * @Description 日志打印工具
 * @date 2022/7/14 10:42
 * @Author Bai XiaoMing
 * @Version 1.0
 */

import hilog from '@ohos.hilog';
const TAG = '[Tetcl Poetry]';
const DOMAIN = 0x6699;

export default class LogUtil {

    /**
     * 打印信息
     * @param tag 页签标记
     * @param log 提示信息
     */
    static printInfo(tag: string, log: string) {
        if (hilog.isLoggable(DOMAIN, TAG, hilog.LogLevel.INFO)) {
            hilog.info(DOMAIN, tag, `----- ${TAG} ---> tag: ** ${tag} ** ---> info: %{public}s -----`, log);
        } else {
            this.printWarning(tag, "领域标识或日志标识暂不支持打印");
        }
    }

    /**
     * 打印警告信息
     * @param tag 页签标记
     * @param log 提示信息
     */
    static printWarning(tag: string, log: string) {
        if (hilog.isLoggable(DOMAIN, TAG, hilog.LogLevel.WARN)) {
            hilog.warn(DOMAIN, tag, `----- ${TAG} ---> tag: ** ${tag} ** ---> warning: %{public}s -----`, log);
        } else {
            console.warn(`${TAG} ---> tag: ${tag} ---> warning: ${log}`);
        }
    }

    /**
     * 打印调试信息
     * @param tag 页签标记
     * @param log 提示信息
     */
    static printDebug(tag: string, log: string) {
        if (hilog.isLoggable(DOMAIN, TAG, hilog.LogLevel.DEBUG)) {
            hilog.debug(DOMAIN, tag, `----- ${TAG} ---> tag: ** ${tag} ** ---> debug: %{public}s -----`, log);
        } else {
            this.printWarning(tag, "领域标识或日志标识暂不支持打印");
        }
    }

    /**
     * 打印错误信息
     * @param tag 页签标记
     * @param log 提示信息
     */
    static printError(tag: string, log: string) {
        if (hilog.isLoggable(DOMAIN, TAG, hilog.LogLevel.ERROR)) {
            hilog.error(DOMAIN, tag, `----- ${TAG} ---> tag: ** ${tag} ** ---> error: %{public}s -----`, log);
        } else {
            this.printWarning(tag, "领域标识或日志标识暂不支持打印");
        }
    }
}