#!/bin/bash
set -e

export RASPBIAN_ROOTFS=$HOME/rpi/rootfs
export PATH=/opt/cross-pi-gcc/bin:/opt/cross-pi-gcc/libexec/gcc/arm-linux-gnueabihf/8.3.0:$PATH
export RASPBERRY_VERSION=1

# HOST=pocketbike
HOST=172.18.15.127

cd build
make -j
#pocketbike
#fe80::bc07:e1b6:baf3:8390%34
ssh pi@$HOST killall pb_display
scp pb_display pi@$HOST:/home/pi
ssh pi@$HOST ./pb_display &