/**
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
 * limitations under the License.  *
 * Description: Provides adc port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#include "mqtt_test.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MQTTClient.h"

#ifdef CMSIS
#include "cmsis_os2.h"
#define LOGI(fmt, ...) printf("[%d] %s " fmt "\n", osKernelGetTickCount(), osThreadGetName(osThreadGetId()), ##__VA_ARGS__)
#else
#define _GNU_SOURCE // for syscall
#include <unistd.h>
#include <sys/syscall.h>
static long gettid(void)
{
    return syscall(SYS_gettid);
}
#define LOGI(fmt, ...) printf("[%ld] " fmt "\n", gettid(), ##__VA_ARGS__)
#endif

#define PUB_MSG_NUM 10

static volatile int running = 1;
static MQTTClient client = {0};
static Network network = {0};
static unsigned char sendbuf[512], readbuf[512];    //增大缓冲区，修复连接华为云失败的问题

#if (defined MQTT_TASK)
static void MqttTask(void* arg)
{
    LOGI("MqttTask start!");
    MQTTClient* c = (MQTTClient*) arg;
    while (c) {
        mqttMutexLock(&c->mutex);
        if (!running) {
            LOGI("MQTT background thread exit!");
            mqttMutexUnlock(&c->mutex);
            break;
        }
        mqttMutexUnlock(&c->mutex);

        mqttMutexLock(&c->mutex);
        if (c->isconnected) {
            // LOGI("checking...");
            MQTTYield(c, 1);
        }
        mqttMutexUnlock(&c->mutex);

        // LOGI("waiting %d %d...", running, c->isconnected);
        Sleep(1000);
        // ThreadYield();
    }
    LOGI("MqttTask exit!");
}
#endif

// in Hi3861 SDK, we can not create task in AT command execution context.
void MqttTestInit(void)
{
    NetworkInit(&network);
    MQTTClientInit(&client, &network, 300, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

#if (defined MQTT_TASK)
    running = 1;
    int rc = ThreadStart(&client.thread, MqttTask, &client);
    LOGI("MqttTestInit ThreadStart: %d", rc);
#endif
    LOGI("MqttTestInit done!");
}

void MqttTestDeinit(void)
{
#if (defined MQTT_TASK)
    mqttMutexLock(&client.mutex);
    running = 0;
    mqttMutexUnlock(&client.mutex);

    ThreadJoin(&client.thread);
#endif
    mqttMutexDeinit(&client.mutex);
}

int MqttTestConnect(const char* host, unsigned short port,
    const char* clientId, const char* username, const char* password)
{
    int rc = 0;
    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

    // connect to server with TCP socket
    if ((rc = NetworkConnect(&network, (char*) host, port)) != 0) {
        LOGI("NetworkConnect is %d", rc);
        return -1;
	}

    if (username != NULL && password != NULL) {
        connectData.username.cstring = (char*) username;
        connectData.password.cstring = (char*) password;
    }
    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = (char*) clientId;

    // send MQTT CONNECT packet
    if ((rc = MQTTConnect(&client, &connectData)) != 0) {
        LOGI("MQTTConnect failed: %d", rc);
        return -1;
    }
    LOGI("MQTT Connected!");
    return 0;
}

static void OnMessageArrived(MessageData* data)
{
    LOGI("Message arrived on topic %.*s: %.*s",
        (int) data->topicName->lenstring.len, (char*) data->topicName->lenstring.data,
        (int) data->message->payloadlen, (char*) data->message->payload);
}

int MqttTestSubscribe(char* topic)
{
    int rc = 0;

    LOGI("Subscribe: #'%s' from broker", topic);
    if ((rc = MQTTSubscribe(&client, topic, QOS2, OnMessageArrived)) != 0) {
        LOGI("MQTTSubscribe failed: %d", rc);
        return -1;
    }
    return 0;
}

int MqttTestUnSubscribe(char* topic)
{
    int rc = 0;

    LOGI("UnSubscribe: #'%s' from broker", topic);
    if ((rc = MQTTUnsubscribe(&client, topic)) != 0) {
        LOGI("MQTTSubscribe failed: %d", rc);
        return -1;
    }
    return 0;
}


int MqttTestPublish(char* topic, char* payload)
{
    int rc = 0;
    MQTTMessage message;

    message.qos = QOS2;
    message.retained = 0;
    message.payload = payload;
    message.payloadlen = strlen(payload);

    LOGI("Publish: #'%s': '%s' to broker", topic, payload);
    if ((rc = MQTTPublish(&client, topic, &message)) != 0) {
        LOGI("MQTTPublish failed: %d", rc);
        return -1;
    }
    return 0;
}

int MqttTestDisconnect(void)
{
    int rc = 0;
    // send MQTT DISCONNECT packet
    if ((rc = MQTTDisconnect(&client)) != 0) {
        LOGI("MQTTDisconnect failed: %d", rc);
        return -1;
    }

    // disconnect TCP socket with server
    NetworkDisconnect(&network);
    return 0;
}

int MqttTestEcho(char* topic)
{
    MqttTestSubscribe(topic);

    for (int i = 1; i <= PUB_MSG_NUM; i++) {
        char payload[30];
        sprintf(payload, "{message_number: %d}", i);
        MqttTestPublish(topic, payload);
    }

#if !(defined MQTT_TASK)
    MQTTYield(&client, 1000);
#endif
    Sleep(200);

    MqttTestUnSubscribe(topic);
    return 0;
}
