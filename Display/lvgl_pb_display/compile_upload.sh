make -j 8
ssh pi@raspi killall pb_display
scp pb_display pi@raspi:/home/pi
ssh pi@raspi ./pb_display &