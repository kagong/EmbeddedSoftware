
arm-none-linux-gnueabi-gcc -static -o switch est.c
adb push switch /data/local/tmp
adb push team_test /data/local/tmp
