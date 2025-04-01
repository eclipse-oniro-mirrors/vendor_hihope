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
# Samples

add_executable(
  hello
  hello.cpp
)
target_include_directories(hello PRIVATE "../../src" "../../src/linux")
target_link_libraries(hello MQTTPacketClient)

add_executable(
  stdoutsub
  stdoutsub.cpp
)
target_include_directories(stdoutsub PRIVATE "../../src" "../../src/linux")
target_link_libraries(stdoutsub paho-embed-mqtt3c)
