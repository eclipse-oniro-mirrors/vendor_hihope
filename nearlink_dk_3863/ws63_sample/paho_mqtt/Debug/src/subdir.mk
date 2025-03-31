################################################################################
# Automatically-generated file. Do not edit!
################################################################################
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MQTTConnectClient.c \
../src/MQTTConnectServer.c \
../src/MQTTDeserializePublish.c \
../src/MQTTPacket.c \
../src/MQTTSerializePublish.c \
../src/MQTTSubscribeClient.c \
../src/MQTTSubscribeServer.c \
../src/MQTTUnsubscribeClient.c \
../src/MQTTUnsubscribeServer.c 

C_DEPS += \
./src/MQTTConnectClient.d \
./src/MQTTConnectServer.d \
./src/MQTTDeserializePublish.d \
./src/MQTTPacket.d \
./src/MQTTSerializePublish.d \
./src/MQTTSubscribeClient.d \
./src/MQTTSubscribeServer.d \
./src/MQTTUnsubscribeClient.d \
./src/MQTTUnsubscribeServer.d 

OBJS += \
./src/MQTTConnectClient.o \
./src/MQTTConnectServer.o \
./src/MQTTDeserializePublish.o \
./src/MQTTPacket.o \
./src/MQTTSerializePublish.o \
./src/MQTTSubscribeClient.o \
./src/MQTTSubscribeServer.o \
./src/MQTTUnsubscribeClient.o \
./src/MQTTUnsubscribeServer.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/icraggs/work/paho/org.eclipse.paho.mqtt.embedded-c/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


