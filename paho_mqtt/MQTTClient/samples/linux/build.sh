#*******************************************************************************
# Copyright (c)  IBM Corp.
# Licensed under the Apache License, Version 2.0 (theLicense");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#      http://www.apache.org/licenses/LICENSE-20
# Unless required by applicable law or agreed to in writing, software
# distributed under the License distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#*******************************************************************************/
g++ hello.cpp -I ../../src/ -I ../../src/linux -I ../../../MQTTPacket/src ../../../MQTTPacket/src/MQTTPacket.c ../../../MQTTPacket/src/MQTTDeserializePublish.c ../../../MQTTPacket/src/MQTTConnectClient.c ../../../MQTTPacket/src/MQTTSubscribeClient.c ../../../MQTTPacket/src/MQTTSerializePublish.c ../../../MQTTPacket/src/MQTTUnsubscribeClient.c -o hello

g++ -g stdoutsub.cpp -I ../../src -I ../../src/linux -I ../../../MQTTPacket/src ../../../MQTTPacket/src/MQTTFormat.c  ../../../MQTTPacket/src/MQTTPacket.c ../../../MQTTPacket/src/MQTTDeserializePublish.c ../../../MQTTPacket/src/MQTTConnectClient.c ../../../MQTTPacket/src/MQTTSubscribeClient.c ../../../MQTTPacket/src/MQTTSerializePublish.c -o stdoutsub ../../../MQTTPacket/src/MQTTConnectServer.c ../../../MQTTPacket/src/MQTTSubscribeServer.c ../../../MQTTPacket/src/MQTTUnsubscribeServer.c ../../../MQTTPacket/src/MQTTUnsubscribeClient.c  
