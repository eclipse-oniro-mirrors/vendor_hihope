#!/bin/bash
#/*
# * Copyright (c) 2023 YourCompany
# * All rights reserved.
# * Unauthorized copying of this file, via any medium strictly prohibited.
# * Proprietary and confidential.
# */
set -e

rm -rf build.paho
mkdir build.paho
cd build.paho
echo "travis build dir $TRAVIS_BUILD_DIR pwd $PWD"
cmake ..
make
python ../test/mqttsas2.py localhost 1883 1885 &
ctest -VV --timeout 600
kill %1
killall mosquitto
