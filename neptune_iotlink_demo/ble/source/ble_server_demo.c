/*
 * Copyright (c) 2021 WinnerMicro Co., Ltd.
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

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "ble_util.h"
#include "my_stdbool.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_gatt_server.h"

enum BLE_GATT_ATTR_MUN {
    ATTR_NUM_0,
    ATTR_NUM_1,
    ATTR_NUM_2,
    ATTR_NUM_3,
    ATTR_NUM_4,
    ATTR_MAX_NUM,
};

static int test_indicate(int *arg);

/* 写数据 */
int char_write(uint8_t *data, int length)
{
    tls_bt_dump_hexstring("char_write:", data, length);
}

/* 读数据 */
int char_read(uint8_t *data, int *length)
{
    const int data_len = 2;

    data[0] = '1';
    data[1] = '1';
    *length = data_len;
}

int char2_write(uint8_t *data, int length)
{
    tls_bt_dump_hexstring("char2_write:", data, length);
}

int char2_read(uint8_t *data, int *length)
{
    const int data_len = 2;

    data[0] = '2';
    data[1] = '2';
    *length = data_len;
}

/* 移除蓝牙服务 */
void test_remove_service(int *arg)
{
    printf("Free server if=0\r\n");
    BleStopAdv(0);
    ble_server_free(0);
    ble_server_start_service();
}

/* 添加蓝牙服务 */
void test_add_service3(void)
{
    int ret, server_if;
    uint16_t uuid;

    uint8_t uuid_128_service[16] = {
        0x00, 0x91, 0x8A, 0xEF, 0x39, 0xDD, 0x84, 0xA4,
        0xFC, 0x43, 0x77, 0xA2, 0x00, 0xE4, 0xF1, 0x15
    };

    uint8_t uuid_128_char1[16] = {
        0x00, 0x91, 0x8A, 0xEF, 0x39, 0xDD, 0x84, 0xA4,
        0xFC, 0x43, 0x77, 0xA2, 0x01, 0xE4, 0xF1, 0x15
    };

    BleGattService service;
    BleGattOperateFunc func;
    BleGattAttr attr[ATTR_MAX_NUM];

    attr[ATTR_NUM_0].attrType = OHOS_BLE_ATTRIB_TYPE_SERVICE;
    attr[ATTR_NUM_0].uuidType = OHOS_UUID_TYPE_128_BIT;
    memcpy_s(attr[ATTR_NUM_0].uuid, sizeof(attr[ATTR_NUM_0].uuid),
        uuid_128_service, sizeof(uuid_128_service));

    attr[ATTR_NUM_1].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR;
    attr[ATTR_NUM_1].uuidType = OHOS_UUID_TYPE_128_BIT;
    memcpy_s(attr[ATTR_NUM_1].uuid, sizeof(attr[ATTR_NUM_1].uuid),
        uuid_128_char1, sizeof(uuid_128_char1));

    func.write = char_write;
    func.read = char_read;
    attr[ATTR_NUM_1].func = func;
    attr[ATTR_NUM_1].properties = 0x0A;
    attr[ATTR_NUM_1].permission = 0x00;

    attr[ATTR_NUM_2].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR;
    attr[ATTR_NUM_2].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x2902;
    memcpy_s(attr[ATTR_NUM_2].uuid, sizeof(attr[ATTR_NUM_2].uuid), &uuid, sizeof(uuid));
    func.write = char_write;
    func.read = char_read;
    attr[ATTR_NUM_2].func = func;
    attr[ATTR_NUM_2].permission = 0x11;

    service.attrNum = ATTR_NUM_3;
    service.attrList = &attr[ATTR_NUM_0];

    ret = BleGattsStartServiceEx(&server_if, &service);
    printf("adding service1, ret=%d, server_if=%d\r\n", ret, server_if);
}

uint8_t indicate_data[128] = {
    0x01, 0x4F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x09, 0x6e,
    0x65, 0x74, 0x43, 0x66, 0x67, 0x56, 0x65, 0x72, 0x09, 0x00,
    0x7b, 0x22, 0x76, 0x65, 0x72, 0x22, 0x3a, 0x32, 0x7d
    };

int test_indicate(int *arg)
{
    (void)arg;
    return OHOS_BT_STATUS_SUCCESS;
}

uint8_t uuid_128_service[16] = {
    0x00, 0x91, 0x8A, 0xEF, 0x39, 0xDD, 0x84, 0xA4,
    0xFC, 0x43, 0x77, 0xA2, 0x00, 0xE6, 0xF1, 0x15
};

uint8_t uuid_128_char1[16] = {
    0x00, 0x91, 0x8A, 0xEF, 0x39, 0xDD, 0x84, 0xA4,
    0xFC, 0x43, 0x77, 0xA2, 0x01, 0xE6, 0xF1, 0x15
};

uint8_t uuid_128_char2[16] = {
    0x00, 0x91, 0x8A, 0xEF, 0x39, 0xDD, 0x84, 0xA4,
    0xFC, 0x43, 0x77, 0xA2, 0x02, 0xE6, 0xF1, 0x15
};

void test_add_service(void)
{
    int ret, server_if;
    uint16_t uuid;

    BleGattService service;
    BleGattOperateFunc func;
    BleGattAttr attr[ATTR_MAX_NUM];

    attr[ATTR_NUM_0].attrType = OHOS_BLE_ATTRIB_TYPE_SERVICE;
    attr[ATTR_NUM_0].uuidType = OHOS_UUID_TYPE_128_BIT;
    memcpy_s(attr[ATTR_NUM_0].uuid, sizeof(attr[ATTR_NUM_0].uuid),
        uuid_128_service, sizeof(uuid_128_service));

    attr[ATTR_NUM_1].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR;
    attr[ATTR_NUM_1].uuidType = OHOS_UUID_TYPE_128_BIT;
    memcpy_s(attr[ATTR_NUM_1].uuid, sizeof(attr[ATTR_NUM_1].uuid),
        uuid_128_char1, sizeof(uuid_128_char1));

    func.write = char_write;
    func.read = char_read;
    attr[ATTR_NUM_1].func = func;
    attr[ATTR_NUM_1].properties = 0x22;
    attr[ATTR_NUM_1].permission = 0x00;

    attr[ATTR_NUM_2].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR;
    attr[ATTR_NUM_2].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x2902;
    memcpy_s(attr[ATTR_NUM_2].uuid, sizeof(attr[ATTR_NUM_2].uuid), &uuid, sizeof(uuid));
    func.write = char_write;
    func.read = char_read;
    attr[ATTR_NUM_2].func = func;
    attr[ATTR_NUM_2].permission = 0x11;

    attr[ATTR_NUM_3].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR;
    attr[ATTR_NUM_3].uuidType = OHOS_UUID_TYPE_128_BIT;
    memcpy_s(attr[ATTR_NUM_3].uuid, sizeof(attr[ATTR_NUM_3].uuid),
        uuid_128_char2, sizeof(uuid_128_char2));

    func.write = char_write;
    func.read = char_read;
    attr[ATTR_NUM_3].func = func;
    attr[ATTR_NUM_3].properties = 0x08;
    attr[ATTR_NUM_3].permission = 0x00;

    attr[ATTR_NUM_4].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR;
    attr[ATTR_NUM_4].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x2902;
    memcpy_s(attr[ATTR_NUM_4].uuid, sizeof(attr[ATTR_NUM_4].uuid), &uuid, sizeof(uuid));
    func.write = char2_write;
    func.read = char2_read;
    attr[ATTR_NUM_4].func = func;
    attr[ATTR_NUM_4].permission = 0x11;

    service.attrNum = ATTR_MAX_NUM;
    service.attrList = &attr[ATTR_NUM_0];

    ret = BleGattsStartServiceEx(&server_if, &service);
    printf("adding service1, ret=%d, server_if=%d\r\n", ret, server_if);
}

void test_add_service2(void)
{
    int ret, server_if;
    uint16_t uuid;

    BleGattService service;
    BleGattOperateFunc func;
    BleGattAttr attr[ATTR_MAX_NUM];
    attr[ATTR_NUM_0].attrType = OHOS_BLE_ATTRIB_TYPE_SERVICE;
    attr[ATTR_NUM_0].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x1826;
    memcpy_s(attr[ATTR_NUM_0].uuid, sizeof(attr[ATTR_NUM_0].uuid), &uuid, sizeof(uuid));

    attr[ATTR_NUM_1].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR;
    attr[ATTR_NUM_1].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x2abc;
    memcpy_s(attr[ATTR_NUM_1].uuid, sizeof(attr[ATTR_NUM_1].uuid), &uuid, sizeof(uuid));
    func.write = char2_write;
    func.read = char2_read;
    attr[ATTR_NUM_1].func = func;
    attr[ATTR_NUM_1].properties = 0x28;
    attr[ATTR_NUM_1].permission = 0x00;
    attr[ATTR_NUM_2].attrType = OHOS_BLE_ATTRIB_TYPE_CHAR;
    attr[ATTR_NUM_2].uuidType = OHOS_UUID_TYPE_16_BIT;
    uuid = 0x2ab8;
    memcpy_s(attr[ATTR_NUM_2].uuid, sizeof(attr[ATTR_NUM_2].uuid), &uuid, sizeof(uuid));
    func.write = char2_write;
    attr[ATTR_NUM_2].func = func;
    attr[ATTR_NUM_2].properties = 0x28;
    attr[ATTR_NUM_2].permission = 0x00;
    service.attrNum = ATTR_NUM_3;
    service.attrList = &attr[ATTR_NUM_0];

    ret = BleGattsStartServiceEx(&server_if, &service);
    printf("adding service2, ret=%d, server_if=%d\r\n", ret, server_if);
}

/* 注册服务的回调 */
void test_registerServerCallback(int status, int serverId, BtUuid *appUuid)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 连接服务的回调 */
void test_connectServerCallback(int connId, int serverId, BdAddr *bdAddr)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 断开服务的回调 */
void test_disconnectServerCallback(int connId, int serverId, BdAddr *bdAddr)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 添加服务的回调 */
void test_serviceAddCallback(int status, int serverId, BtUuid *uuid, int srvcHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

void test_includeServiceAddCallback(int status, int serverId, int srvcHandle, int includeSrvcHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

void test_characteristicAddCallback(int status, int serverId, BtUuid *uuid,
    int srvcHandle, int characteristicHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

void test_descriptorAddCallback(int status, int serverId, BtUuid *uuid,
    int srvcHandle, int descriptorHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 启动服务的回调 */
void test_serviceStartCallback(int status, int serverId, int srvcHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 停止服务的回调 */
void test_serviceStopCallback(int status, int serverId, int srvcHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 删除服务的回调 */
void test_serviceDeleteCallback(int status, int serverId, int srvcHandle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, serverId);
}

/* 读数据请求回调 */
void test_requestReadCallback(BtReqReadCbPara readCbPara)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, 0);
}

/* 写数据请求的回调 */
void test_requestWriteCallback(BtReqWriteCbPara writeCbPara)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, 0);
}

/* 确认回复的回调 */
void test_responseConfirmationCallback(int status, int handle)
{
    printf("%s serverId=%d\r\n", __FUNCTION__, 0);
}

void test_indicationSentCallback(int connId, int status)
{
    printf("%s connId=%d\r\n", __FUNCTION__, connId);
}

/* 修改最大传输单元的数值 */
void test_mtuChangeCallback(int connId, int mtu)
{
    printf("%s connId=%d\r\n", __FUNCTION__, connId);
}

static BtGattServerCallbacks scb = {
    test_registerServerCallback,
    test_connectServerCallback,
    test_disconnectServerCallback,
    test_serviceAddCallback,
    test_includeServiceAddCallback,
    test_characteristicAddCallback,
    test_descriptorAddCallback,
    test_serviceStartCallback,
    test_serviceStopCallback,
    test_serviceDeleteCallback,
    test_requestReadCallback,
    test_requestWriteCallback,
    test_responseConfirmationCallback,
    test_indicationSentCallback,
    test_mtuChangeCallback,
};

/* 使能广播的回调 */
void test_gap_adv_enable_callback(int advId, int status)
{
    (void)advId;
    printf("%s status=%d\r\n", __FUNCTION__, status);
}

/* 禁用广播的回调 */
void test_gap_adv_disable_callback(int advId, int status)
{
    (void)advId;
    printf("%s status=%d\r\n", __FUNCTION__, status);
}

/* 设置广播数据的回调 */
void test_gap_adv_data_set_callback(int advId, int status)
{
    (void)advId;
    printf("%s status=%d\r\n", __FUNCTION__, status);
}

/* 广播更新回调 */
void test_gap_adv_update_callback(int advId, int status)
{
    (void)advId;
    printf("%s status=%d\r\n", __FUNCTION__, status);
}

void test_gap_sec_response_callback(const BdAddr *bdAddr)
{
    printf("%s addr ", __FUNCTION__);
    for (int i = 0; i < OHOS_BD_ADDR_LEN; i++) printf("%02x:", bdAddr->addr[i]);
    printf("\r\n");
}

/* 扫描结果的回调 */
void test_gap_scan_result_callback(BtScanResultData *scanResultdata)
{
    int i;

    printf("scan result: ");
    for (int i = 0; i < OHOS_BD_ADDR_LEN; i++) printf("%02x:", scanResultdata->addr.addr[i]);
    printf("\r\n");

    for (int i = 0; i < scanResultdata->advLen; i++) printf("%02x", scanResultdata->advData[i]);
    printf("\r\n");
}

/* 扫描参数设置的回调测试 */
void test_gap_scan_param_set_callback(int clientId, int status)
{
    (void)clientId;
    printf("%s status=%d\r\n", __FUNCTION__, status);
}

static BtGattServerCallbacks gcb = {
    test_gap_adv_enable_callback,
    test_gap_adv_disable_callback,
    test_gap_adv_data_set_callback,
    test_gap_adv_update_callback,
    test_gap_sec_response_callback,
    test_gap_scan_result_callback,
    test_gap_scan_param_set_callback,
};

void ble_server_demo(void)
{
    StartAdvRawData raw;
    BleAdvParams param;

    memset_s(&raw, sizeof(raw), 0, sizeof(raw));
    memset_s(&param, sizeof(param), 0, sizeof(param));

    uint8_t adv_data[] = {
        0x02, 0x01, 0x06, 0x15, 0x16, 0xEE, 0xFD, 0x01, 0x01, 0x07, 0x04, 0x00, 0x11,
        0xF8, 0x12, 0x44, 0x30, 0x30, 0x32, 0xFF, 0x00, 0x04, 0x02, 0x02, 0x01
    };

    uint8_t scan_data[] = {
        0x0B, 0x09,
        'H', 'e', 'l', 'l', 'o', ',', 'B', 'L', 'E', '!'
    };

    raw.advDataLen = sizeof(adv_data)/sizeof(adv_data[0]);
    raw.advData = adv_data;

    raw.rspDataLen = sizeof(scan_data)/sizeof(scan_data[0]);
    raw.rspData = scan_data;

    param.advFilterPolicy = 0;
    param.advType = OHOS_BLE_ADV_IND;
    param.channelMap = 0x07;
    param.ownAddrType = 0;
    param.duration = 0;
    param.maxInterval = 0x60;
    param.minInterval = 0x40;

    /* register gap callback */
    BleGattRegisterCallbacks(&gcb);

    /* register gatt server callback */
    BleGattsRegisterCallbacks(&scb);

    printf("Adding service....\r\n");

    test_add_service();
    printf("Runnng service....\r\n");

    printf("Starting advertisment...\r\n");
    BleStartAdvEx(1, raw, param);
}
