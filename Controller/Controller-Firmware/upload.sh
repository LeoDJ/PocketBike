#!/bin/bash

ssh pi@pocketbike stty -F /dev/serial/by-path/platform-20980000.usb-usb-0:1:1.6 1200
ssh pi@pocketbike sudo usbreset 2e8a:00c0
sleep 7
scp .pio/build/pico/firmware.uf2 pi@pocketbike:/mnt/pico