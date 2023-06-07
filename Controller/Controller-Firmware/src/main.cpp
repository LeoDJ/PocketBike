#include <Arduino.h>

#include <VescUart.h>
#include <NeoPixelConnect.h>

#include "config.h"

REDIRECT_STDOUT_TO(Serial); // activate printf

#include <USB/PluggableUSBSerial.h>
USBSerial SerialUSB0_VESC(false, "VESC passthrough");
USBSerial SerialUSB1_GPS(false, "GPS passthrough");
USBSerial SerialUSB2_BMS(false, "BMS passthrough"); // reserved

USBSerial *usbSerials[] = {&SerialUSB0_VESC, &SerialUSB1_GPS, &SerialUSB2_BMS, &_SerialUSB}; // USB endpoint order
const int usbSerialsNum = sizeof(usbSerials) / sizeof(usbSerials)[0];
bool usbSerialsState[usbSerialsNum] = {0};
// uint32_t usbSerialsLastClose[usbSerialsNum] = {0};

// manually define hardware UARTs, as the Arduino Framework still doesn't support Serial2
UART uart0_VESC(PIN_VESC_TX, PIN_VESC_RX);
UART uart1_GPS(PIN_GPS_TX, PIN_GPS_RX);


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
    uart0_VESC.begin(VESC_BAUD);
    vesc.setSerialPort(&uart0_VESC);
    // vesc.setDebugPort(Serial);

    uart1_GPS.begin(GPS_BAUD);

    SerialUSB0_VESC.begin(0);
    SerialUSB1_GPS.begin(0);

    pinMode(LED_BUILTIN, OUTPUT);

    // display test pattern
    led.neoPixelSetValue(0, 255, 0, 0);
    led.neoPixelSetValue(1, 0, 255, 0);
    led.neoPixelSetValue(2, 0, 0, 255);
    led.neoPixelSetValue(3, 255, 255, 255);
    led.neoPixelShow();
}

uint32_t lastVescPoll = 0;

void doUsbSerialPassthrough(USBSerial *usbSerial, UART *hwSerial, int usbSerialId) {
    if (usbSerial->connected()) {
        while (usbSerial->available()) {
            char buf[64];
            size_t numBytes = min(usbSerial->available(), (int)sizeof(buf));
            usbSerial->readBytes(buf, numBytes);
            hwSerial->write(buf, numBytes);
            printf("Forwarding %d >> ", usbSerialId);
            for (size_t i = 0; i < numBytes; i++) {
                printf("%02X ", buf[i]);
            }
            printf("\n");
        }
        while (hwSerial->available()) {
            char buf[64];
            size_t numBytes = min(hwSerial->available(), (int)sizeof(buf));
            hwSerial->readBytes(buf, numBytes);
            usbSerial->write(buf, numBytes);
            printf("Forwarding %d << ", usbSerialId);
            for (size_t i = 0; i < numBytes; i++) {
                printf("%02X ", buf[i]);
            }
            printf("\n");
        }
    }
}



void loop() {
    if (millis() - lastVescPoll > VESC_POLL_INTERVAL) {
        lastVescPoll = millis();
        if (!SerialUSB0_VESC.connected()) { // check if passthrough is not active
            vescPoll();
        }
        // printf("Connected: %d (%d), %d (%d), %d (%d)\n", 
        //     SerialUSB0_VESC.connected(), 
        //     SerialUSB0_VESC.available(), 
        //     SerialUSB1_GPS.connected(), 
        //     SerialUSB1_GPS.available(), 
        //     SerialUSB2_BMS.connected(),
        //     SerialUSB2_BMS.available()
        // );
        // SerialUSB0_VESC.println("2");
        // SerialUSB1_GPS.println("3");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    for (int i = 0; i < usbSerialsNum; i++) {
        if (usbSerials[i]->connected() != usbSerialsState[i]) {
            printf("USB Serial %d %sconnected\n", i, usbSerialsState[i] ? "dis" : "");
            // if (usbSerialsState[i]) {
            //     usbSerialsLastClose[i] = millis();
            // }
            usbSerialsState[i] = usbSerials[i]->connected();
        }
    }
    

    doUsbSerialPassthrough(&SerialUSB0_VESC, &uart0_VESC, 0);
    doUsbSerialPassthrough(&SerialUSB1_GPS, &uart1_GPS, 1);
    
}