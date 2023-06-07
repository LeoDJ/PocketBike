#!/bin/bash

ssh pi@raspi stty -F /dev/serial/by-path/platform-20980000.usb-usb-0:1:1.6 1200
sleep 7
scp .pio/build/pico/firmware.uf2 pi@raspi:/mnt/pico