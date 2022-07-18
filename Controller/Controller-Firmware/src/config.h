#pragma once

const int PIN_UART0_TX = 0;
const int PIN_UART0_RX = 1;
const int PIN_UART1_TX = 4;
const int PIN_UART1_RX = 5;

const int VESC_BAUD = 19200;
const int VESC_POLL_INTERVAL = 200;     // ms

const int PIN_WS2812 = 2;
const int NUM_LEDS = 180;
const PIO WS2812_PIO = pio0;
const int WS2812_PIO_SM = 0;