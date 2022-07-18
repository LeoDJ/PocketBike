#include <Arduino.h>

#include <VescUart.h>
#include <NeoPixelConnect.h>

#include "config.h"

REDIRECT_STDOUT_TO(Serial); // activate printf

// manually define hardware UARTs, as the Arduino Framework still doesn't support Serial2
UART UART0(PIN_UART0_TX, PIN_UART0_RX);
UART UART1(PIN_UART1_TX, PIN_UART1_RX);

VescUart vesc;

NeoPixelConnect led(PIN_WS2812, NUM_LEDS, WS2812_PIO, WS2812_PIO_SM);


void vescPoll() {
    if (vesc.getVescValues()) {
        // vesc.printVescValues();
        printf("vesc_raw_values;avgMotorCurrent=%.6g;avgInputCurrent=%.6g;dutyCycleNow=%.6g;rpm=%.6g;inpVoltage=%.6g;ampHours=%.6g;ampHoursCharged=%.6g;wattHours=%.6g;wattHoursCharged=%.6g;tachometer=%ld;tachometerAbs=%ld;tempMosfet=%.6g;tempMotor=%.6g;pidPos=%.6g;id=%d;error=%d;\n",
            vesc.data.avgMotorCurrent,
            vesc.data.avgInputCurrent,
            vesc.data.dutyCycleNow,
            vesc.data.rpm,
            vesc.data.inpVoltage,
            vesc.data.ampHours,
            vesc.data.ampHoursCharged,
            vesc.data.wattHours,
            vesc.data.wattHoursCharged,
            vesc.data.tachometer,
            vesc.data.tachometerAbs,
            vesc.data.tempMosfet,
            vesc.data.tempMotor,
            vesc.data.pidPos,
            vesc.data.id,
            vesc.data.error
        );

        // TODO: decide where to do the data processing and send respective values
    }
}


void setup() {
    // Init VESC UART
    UART0.begin(VESC_BAUD);
    vesc.setSerialPort(&UART0);
    // vesc.setDebugPort(Serial);

    // display test pattern
    led.neoPixelSetValue(0, 255, 0, 0);
    led.neoPixelSetValue(1, 0, 255, 0);
    led.neoPixelSetValue(2, 0, 0, 255);
    led.neoPixelSetValue(3, 255, 255, 255);
    led.neoPixelShow();
}

uint32_t lastVescPoll = 0;

void loop() {
    if (millis() - lastVescPoll > VESC_POLL_INTERVAL) {
        lastVescPoll = millis();
        vescPoll();
    }
}