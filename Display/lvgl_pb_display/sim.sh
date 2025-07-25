#!/bin/bash
set -e

cmake CMakeLists.txt -B ./build_sim -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDO_PROFILE=0 -DBUILD_SIM=1
cd build_sim
make -j
./pb_display