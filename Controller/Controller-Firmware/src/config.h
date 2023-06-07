#pragma once

const int PIN_LIGHT_SENSOR = 28;
const int PIN_HEADLIGHT_LOW = 20;
const int PIN_HEADLIGHT_HIGH = 21;

const int PIN_VESC_TX = 16;
const int PIN_VESC_RX = 17;
const int VESC_BAUD = 115200;
const int VESC_POLL_INTERVAL = 200;     // ms

const int PIN_GPS_TX = 4;
const int PIN_GPS_RX = 5;
const int GPS_BAUD = 115200;

const int PIN_WS2812 = 15;
const int NUM_LEDS = 180;
const PIO WS2812_PIO = pio0;
const int WS2812_PIO_SM = 0;


// pre-reserved, not used yet
const int PIN_IMU_SCK = 10;
const int PIN_IMU_MOSI = 11;
const int PIN_IMU_MISO = 12;
const int PIN_IMU_CS = 13;
const int PIN_BMS_TX = 2;
const int PIN_BMS_RX = 3;