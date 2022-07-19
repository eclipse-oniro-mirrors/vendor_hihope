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
 * @Description 请求数据工具
 * @date 2022/7/18 9:36
 * @Author Bai XiaoMing
 * @Version 1.0
 */

import prompt from '@ohos.prompt';
import http from '@ohos.net.http';

import RestApi from './RestApiUtil';
import log from './LogUtil';

const TAG = "RequestUtil";

class RequestUtil {

    get(url: string, data?: any): Promise<any> {
        return new Promise((resolve) => {
            this.request(url, http.RequestMethod.GET, data).then((res) => {
                resolve(res);
            })
        })
    }

    post(url: string, data?: any): Promise<any> {
        return new Promise((resolve) => {
            this.request(url, http.RequestMethod.POST, data).then((res) => {
                resolve(res);
            })
        })
    }

    private request(url: string, method: http.RequestMethod.GET | http.RequestMethod.POST, data?: any): Promise<any> {
        return new Promise((resolve) => {
            let request = http.createHttp();
            request.request(url, {
                method,
                header: {
                    "Content-Type": method === http.RequestMethod.GET ? "application/json" : "application/x-www-form-urlencoded;charset=UTF-8"
                },
                extraData: data
            }).then((res) => {
                log.printInfo(TAG, JSON.stringify(res));
                let { responseCode, result } = res;
                if (responseCode === 200) {
                    log.printInfo(TAG, result.toString());
                    let data = result.toString();
                    let resultVal: RestApi = JSON.parse(data);
                    resolve(resultVal);
                } else {
                    prompt.showToast({
                        message: "HTTP级异常：" + res.responseCode,
                        duration: 2000
                    })
                }
            }).catch((err) => {
                prompt.showToast({
                    message: "系统级异常：" + JSON.stringify(err),
                    duration: 2000
                })
            })
        })
    }
}

let request = new RequestUtil();
export default request;