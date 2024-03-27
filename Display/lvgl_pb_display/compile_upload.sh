export RASPBIAN_ROOTFS=$HOME/rpi/rootfs
export PATH=/opt/cross-pi-gcc/bin:/opt/cross-pi-gcc/libexec/gcc/arm-linux-gnueabihf/8.3.0:$PATH
export RASPBERRY_VERSION=1

cd build
make -j
#pocketbike
#fe80::bc07:e1b6:baf3:8390%34
ssh pi@pocketbike killall pb_display
scp pb_display pi@pocketbike:/home/pi
ssh pi@pocketbike ./pb_display &