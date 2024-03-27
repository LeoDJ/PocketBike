#pragma once

#include <stdint.h>
#include <ArduinoJson.hpp>

typedef struct ValuesSetupPackage {
    float           tempMosfet;
    float           tempMotor;
    float           motorCurrent;
    float           inputCurrent;
    float           dutyCycleNow;
    float           rpm;
    float           speed;
    float           inpVoltage;
    float           batteryLevel;
    float           ampHours;
    float           ampHoursCharged;
    float           wattHours;
    float           wattHoursCharged;
    float           distance;
    float           distanceAbs;
    float           pidPos;
    uint8_t         error;
    uint8_t         id;
    uint8_t         numVescs;
    float           wattHoursLeft;
    uint32_t        odometer;
    uint32_t        uptimeMs;
} ValuesSetupPackage_t;


// does C++ ADL magic to define a converter for ArduinoJson
void convertFromJson(JsonVariantConst src, ValuesSetupPackage_t& dst) {
    dst.tempMosfet =        src["tempMosfet"].as<float>();
    dst.tempMotor =         src["tempMotor"].as<float>();
    dst.motorCurrent =      src["motorCurrent"].as<float>();
    dst.inputCurrent =      src["inputCurrent"].as<float>();
    dst.dutyCycleNow =      src["dutyCycleNow"].as<float>();
    dst.rpm =               src["rpm"].as<float>();
    dst.speed =             src["speed"].as<float>();
    dst.inpVoltage =        src["inpVoltage"].as<float>();
    dst.batteryLevel =      src["batteryLevel"].as<float>();
    dst.ampHours =          src["ampHours"].as<float>();
    dst.ampHoursCharged =   src["ampHoursCharged"].as<float>();
    dst.wattHours =         src["wattHours"].as<float>();
    dst.wattHoursCharged =  src["wattHoursCharged"].as<float>();
    dst.distance =          src["distance"].as<float>();
    dst.distanceAbs =       src["distanceAbs"].as<float>();
    dst.pidPos =            src["pidPos"].as<float>();
    dst.error =             src["error"].as<uint8_t>();
    dst.id =                src["id"].as<uint8_t>();
    dst.numVescs =          src["numVescs"].as<uint8_t>();
    dst.wattHoursLeft =     src["wattHoursLeft"].as<float>();
    dst.odometer =          src["odometer"].as<uint32_t>();
    dst.uptimeMs =          src["uptimeMs"].as<uint32_t>();
}