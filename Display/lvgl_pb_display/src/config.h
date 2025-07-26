#pragma once
#include "config_secret.h"

static const char *serialPortPath = "/dev/serial/by-path/platform-20980000.usb-usb-0:1:1.6";
static const int serialPortBaud = 115200;

static const int UPDATE_RATE = 100;  // ms

static const int PIN_BL_WHITE = 23;
static const int PIN_BL_RED = 24;