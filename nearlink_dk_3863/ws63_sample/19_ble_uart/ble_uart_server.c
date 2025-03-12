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
#include "osal_addr.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "product.h"
#include "securec.h"
#include "errcode.h"
#include "uart.h"
#include "bts_def.h"
#include "ohos_init.h" 
#include "cmsis_os2.h" 
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "ble_uart_server_adv.h"
#include "ble_uart_server.h"
#include "bts_gatt_server.h"
#include "ble_uart_server.h"

/* uart gatt server id */
#define BLE_UART_SERVER_ID 			1
/* uart ble connect id */
#define BLE_SINGLE_LINK_CONNECT_ID 	1
/* octets of 16 bits uart */
#define UART16_LEN 					2
/* invalid attribute handle */
#define INVALID_ATT_HDL 			0
/* invalid server ID */
#define INVALID_SERVER_ID 			0

#define BLE_UART_SERVER_LOG "[ble uart server]"
#define BLE_UART_SERVER_ERROR "[ble uart server error]"
#define BLE_UART_SERVICE_NUM 3
#define BLE_UART_TASK_STACK_SIZE            0x1200
#define BLE_UART_TASK_PRIO                  28
#define BLE_UART_BT_STACK_POWER_MS      10000
#define CONFIG_BLE_UART_BUS 0

static uint16_t g_ble_uart_conn_id;
static uint8_t g_ble_uart_name_value[] = { 'b', 'l', 'e', '_', 'u', 'a', 'r', 't', '\0' };
static uint8_t g_uart_server_app_uuid[] = { 0x00, 0x00 };
static uint8_t g_ble_uart_server_addr[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
static uint8_t g_server_id = INVALID_SERVER_ID;
static uint8_t g_connection_state = 0;
static uint16_t g_notify_indicate_handle = 0;
static uint8_t g_service_num = 0;
typedef struct {
    uint8_t *value;
    uint16_t value_len;
} msg_data_t;
static unsigned long mouse_msg_queue = 0;
static unsigned int msg_rev_size = sizeof(msg_data_t);

static void ble_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    printf("ble_uart_read_int_handler server.\r\n");
    unused(error);
    if (ble_uart_get_connection_state() != 0) {
        msg_data_t msg_data = { 0 };
        void* buffer_cpy = osal_vmalloc(length);
        if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
            osal_vfree(buffer_cpy);
            return;
        }
        msg_data.value = (uint8_t *)buffer_cpy;
        msg_data.value_len = length;
        osal_msg_queue_write_copy(mouse_msg_queue, (void *)&msg_data, msg_rev_size, 0);
    }
}

static void *ble_uart_server_task(const char *arg)
{
    unused(arg);
    ble_uart_server_init();
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_BLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_IDLE,
                                                   1, ble_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        printf("Register uart callback fail.");
        return NULL;
    }
    int c = 1;
    while (c) {
        msg_data_t msg_data = { 0 };
        int msg_ret = osal_msg_queue_read_copy(mouse_msg_queue, &msg_data, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (msg_ret != OSAL_SUCCESS) {
            printf("msg queue read copy fail.");
            if (msg_data.value != NULL) {
                osal_vfree(msg_data.value);
            }
            continue;
        }
        if (msg_data.value != NULL) {
            ble_uart_server_send_input_report(msg_data.value, msg_data.value_len);
            osal_vfree(msg_data.value);
        }
    }
    return NULL;
}

/* 将uint16的uuid数字转化为bt_uuid_t */
static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = UART16_LEN;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

/* 设置注册服务时的name */
void ble_uart_set_device_name_value(const uint8_t *name, const uint8_t len)
{
    size_t len_name = sizeof(g_ble_uart_name_value);
    if (memcpy_s(g_ble_uart_name_value, len_name, name, len) != EOK) {
        printf("%s memcpy name fail\n", BLE_UART_SERVER_ERROR);
    }
}

/* 创建服务 */
static void ble_uart_add_service(void)
{
    bt_uuid_t uart_service_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UART_UUID_SERVER_SERVICE, &uart_service_uuid);
    gatts_add_service(BLE_UART_SERVER_ID, &uart_service_uuid, true);
}

/* 添加uart发送服务的所有特征和描述符 */
static void ble_uart_add_tx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    printf("%s TX characters:%d srv_handle:%d \n", BLE_UART_SERVER_LOG, server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x12, 0x34 };
    bts_data_to_uuid_len2(BLE_UART_CHARACTERISTIC_UUID_TX, &characters_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_NOTIFY | GATT_CHARACTER_PROPERTY_BIT_READ;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    printf("%s characters_uuid:%2x %2x\n", BLE_UART_SERVER_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);

    static uint8_t ccc_val[] = { 0x01, 0x00 }; // notify
    bt_uuid_t ccc_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UART_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE |
        GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_val);
    descriptor.value = ccc_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    printf("%s ccc_uuid:%2x %2x\n", BLE_UART_SERVER_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);
}

/* 添加uart接收服务的所有特征和描述符 */
static void ble_uart_add_rx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    printf("%s RX characters:%d srv_handle: %d \n", BLE_UART_SERVER_LOG, server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x12, 0x34 };
    bts_data_to_uuid_len2(BLE_UART_CHARACTERISTIC_UUID_RX, &characters_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    printf("%s characters_uuid:%2x %2x\n", BLE_UART_SERVER_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);

    bt_uuid_t ccc_uuid = { 0 };
    /* uart client characteristic configuration value for test */
    static uint8_t ccc_val[] = { 0x00, 0x00 };
    bts_data_to_uuid_len2(BLE_UART_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_val);
    descriptor.value = ccc_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    printf("%s ccc_uuid:%2x %2x\n", BLE_UART_SERVER_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);
}

bool bts_uart_compare_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return false;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return false;
    }
    return true;
}

/* 服务添加回调 */
static void ble_uart_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    printf("%s add characters_and_descriptors cbk service:%d, srv_handle:%d, uuid_len:%d, status:%d, uuid:",
                BLE_UART_SERVER_LOG, server_id, handle, uuid->uuid_len, status);
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        printf("%02x ", uuid->uuid[i]);
    }
    printf("\n");
    ble_uart_add_tx_characters_and_descriptors(server_id, handle);
    ble_uart_add_rx_characters_and_descriptors(server_id, handle);
    gatts_start_service(server_id, handle);
}

/* 特征添加回调 */
static void ble_uart_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
                                                   gatts_add_character_result_t *result, errcode_t status)
{
    printf("%s add character cbk service:%d service_hdl: %d char_hdl: %d char_val_hdl: %d uuid_len: %d \n",
                BLE_UART_SERVER_LOG, server_id, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    printf("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        printf("%02x ", uuid->uuid[i]);
    }
    bt_uuid_t characters_cbk_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UART_CHARACTERISTIC_UUID_TX, &characters_cbk_uuid);
    characters_cbk_uuid.uuid_len = uuid->uuid_len;
    if (bts_uart_compare_uuid(uuid, &characters_cbk_uuid)) {
        g_notify_indicate_handle = result->value_handle;
    }
    printf("%s status:%d indicate_handle:%d\n", BLE_UART_SERVER_LOG, status, g_notify_indicate_handle);
}

/* 描述符添加回调 */
static void ble_uart_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
                                               uint16_t handle, errcode_t status)
{
    printf("%s service:%d service_hdl: %d desc_hdl: %d uuid_len: %d \n",
                BLE_UART_SERVER_LOG, server_id, service_handle, handle, uuid->uuid_len);
    printf("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        printf("%02x ", (uint8_t)uuid->uuid[i]);
    }
    printf("%s status:%d\n", BLE_UART_SERVER_LOG, status);
}

/* 开始服务回调 */
static void ble_uart_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    g_service_num++;
    if ((g_service_num == BLE_UART_SERVICE_NUM) && (status == 0)) {
        printf("%s start service cbk , start adv\n", BLE_UART_SERVER_LOG);
        ble_uart_set_adv_data();
        ble_uart_start_adv();
    }
    printf("%s start service:%2d service_hdl: %d status: %d\n",
                BLE_UART_SERVER_LOG, server_id, handle, status);
}

static void ble_uart_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    printf("%s ble uart write cbk server_id:%d, conn_id:%d, status%d\n",
        BLE_UART_SERVER_LOG, server_id, conn_id, status);
    printf("%s ble uart write cbk len:%d, data:%s\n",
                BLE_UART_SERVER_LOG, write_cb_para->length, write_cb_para->value);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        uapi_uart_write(CONFIG_BLE_UART_BUS, (uint8_t *)(write_cb_para->value), write_cb_para->length, 0);
    }
}

static void ble_uart_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    printf("%s ReceiveReadReq--server_id:%d conn_id:%d\n", BLE_UART_SERVER_LOG, server_id, conn_id);
    printf("%s request_id:%d, att_handle:%d offset:%d, need_rsp:%d, is_long:%d\n",
                BLE_UART_SERVER_LOG, read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset,
                read_cb_para->need_rsp, read_cb_para->is_long);
    printf("%s status:%d\n", BLE_UART_SERVER_LOG, status);
}

static void ble_uart_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    printf("%s MtuChanged--server_id:%d conn_id:%d\n", BLE_UART_SERVER_LOG, server_id, conn_id);
    printf("%s mtusize:%d, status:%d\n", BLE_UART_SERVER_LOG, mtu_size, status);
}

static void ble_uart_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    printf("%s adv enable cbk adv_id:%d status:%d\n", BLE_UART_SERVER_LOG, adv_id, status);
}

static void ble_uart_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    printf("%s adv disable adv_id: %d, status:%d\n", BLE_UART_SERVER_LOG, adv_id, status);
}

void ble_uart_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_ble_uart_conn_id = conn_id;
    g_connection_state = (uint8_t)conn_state;
    printf("%s connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
                BLE_UART_SERVER_LOG, conn_id, conn_state, pair_state, addr[0], disc_reason);
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        return;
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        ble_uart_set_adv_data();
        ble_uart_start_adv();
    }
}
void ble_uart_server_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    printf("%s pair result conn_id: %d, status: %d, addr %x \n",
                BLE_UART_SERVER_LOG, conn_id, status, addr[0]);
}

static errcode_t ble_uart_server_register_callbacks(void)
{

    gap_ble_callbacks_t gap_cb = { 0 };
    gatts_callbacks_t service_cb = { 0 };
    gap_cb.start_adv_cb = ble_uart_server_adv_enable_cbk;
    gap_cb.conn_state_change_cb = ble_uart_server_connect_change_cbk;
    gap_cb.stop_adv_cb = ble_uart_server_adv_disable_cbk;
    gap_cb.pair_result_cb = ble_uart_server_pair_result_cb;
    errcode_t ret = gap_ble_register_callbacks(&gap_cb);


    service_cb.add_service_cb = ble_uart_server_service_add_cbk;
    service_cb.add_characteristic_cb = ble_uart_server_characteristic_add_cbk;
    service_cb.add_descriptor_cb = ble_uart_server_descriptor_add_cbk;
    service_cb.start_service_cb = ble_uart_server_service_start_cbk;
    service_cb.read_request_cb = ble_uart_receive_read_req_cbk;
    service_cb.write_request_cb = ble_uart_receive_write_req_cbk;
    service_cb.mtu_changed_cb = ble_uart_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        printf("%s reg service cbk failed ret = %d\n", BLE_UART_SERVER_ERROR, ret);
        return ret;
    }
    return ret;
}

void ble_uart_server_init(void)
{
    (void)osal_msleep(3000); /* 延时3s，等待SLE初始化完毕 */
    ble_uart_server_register_callbacks();
    enable_ble();

    errcode_t ret = 0;
    bt_uuid_t app_uuid = { 0 };
    bd_addr_t ble_addr = { 0 };
    app_uuid.uuid_len = sizeof(g_uart_server_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_uart_server_app_uuid, sizeof(g_uart_server_app_uuid)) != EOK) {
        printf("%s add server app uuid memcpy failed\n", BLE_UART_SERVER_ERROR);
        return;
    }
    ble_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_ble_uart_server_addr, sizeof(g_ble_uart_server_addr)) != EOK) {
        printf("%s add server app addr memcpy failed\n", BLE_UART_SERVER_ERROR);
        return;
    }
    gap_ble_set_local_name(g_ble_uart_name_value, sizeof(g_ble_uart_name_value));
    gap_ble_set_local_addr(&ble_addr);
    ret = gatts_register_server(&app_uuid, &g_server_id);
    if ((ret != ERRCODE_BT_SUCCESS) || (g_server_id == INVALID_SERVER_ID)) {
        printf("%s add server failed\r\n", BLE_UART_SERVER_ERROR);
        return;
    }
    ble_uart_add_service(); /* 添加uart服务 */
    printf("%s beginning add service\r\n", BLE_UART_SERVER_LOG);
    bth_ota_init();

}

/* device向host发送数据：input report */
errcode_t ble_uart_server_send_input_report(uint8_t *data, uint16_t len)
{
    gatts_ntf_ind_t param = { 0 };
    uint16_t conn_id = g_ble_uart_conn_id;
    param.attr_handle = g_notify_indicate_handle;
    param.value_len = len;
    param.value = data;
    printf("%s send input report indicate_handle:%d\n", BLE_UART_SERVER_LOG, g_notify_indicate_handle);
    gatts_notify_indicate(BLE_UART_SERVER_ID, conn_id, &param);
    return ERRCODE_BT_SUCCESS;
}

uint8_t ble_uart_get_connection_state(void)
{
    return g_connection_state;
}

static void ble_uart_entry(void)
{

    int msg_ret = osal_msg_queue_create("task_msg", msg_rev_size, &mouse_msg_queue, 0, msg_rev_size);
    if (msg_ret != OSAL_SUCCESS) {
        printf("msg queue create fail.");
        return;
    }
    osal_task *task_handle = NULL;
    osThreadAttr_t attr;
    attr.name = "BleServerTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLE_UART_TASK_STACK_SIZE;
    attr.priority = BLE_UART_TASK_PRIO;
    osal_kthread_lock();
    if(osThreadNew((osThreadFunc_t)ble_uart_server_task, NULL,&attr) == NULL){
        printf("[BleServerLog] Failed to create BleServerTask!\n");
    }else{
        printf("[BleServerLog] create BleServerTask successfully!\n");
    }
    osal_kthread_unlock();
}

/* Run the ble_uart_entry. */
APP_FEATURE_INIT(ble_uart_entry);