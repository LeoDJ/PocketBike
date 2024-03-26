#include <VescUart.h>
#include <ArduinoJson.h>

VescUart vesc;

uint32_t lastTransition = 0;
int transitionNum = 0;
uint32_t leverTimeout = 1000, leverDebounce = 50;
bool headlightState = false;

// hacky lever codes
void handleInputs(float gas, float brake) {
    if (brake > 0.8) {
        if ((gas > 0.5 && transitionNum % 2 == 0) ||
            (gas < 0.2 && transitionNum % 2 == 1)) {
            
            if (transitionNum > 0 && millis() - lastTransition > leverTimeout) { // if not first transition and over timeout, reset count and start over
                transitionNum = 0;
                return;
            }

            // TODO: debounce?

            transitionNum++;
            lastTransition = millis();
        }
    }
    if (transitionNum > 0 && (brake < 0.8 || millis() - lastTransition >= leverTimeout)) {
        switch (transitionNum) {
            case 6: // 3x pulled
                if (headlightState) {
                    digitalWrite(PIN_HEADLIGHT_HIGH, LOW);
                    analogWrite(PIN_HEADLIGHT_LOW, 0);
                }
                else {
                    digitalWrite(PIN_HEADLIGHT_HIGH, HIGH);
                    analogWrite(PIN_HEADLIGHT_LOW, HEADLIGHT_LOWBEAM_PWM);
                }
                headlightState = !headlightState;
                break;
        }
        transitionNum = 0;
    }
        
}


void vescPoll() {

    // if (vesc.getMcConfValues()) {
    //     printf("vesc_raw_mcconf;ICMin=%.6g;ICMax=%.6g;MP=%d;GR=%.6g;WD=%.6g\n", 
    //         vesc.mcconf.inCurrentMin,
    //         vesc.mcconf.inCurrentMax,
    //         vesc.mcconf.motorPoles,
    //         vesc.mcconf.gearRatio,
    //         vesc.mcconf.wheelDiameter
    //         );
    // }

    if (vesc.getSetupValues()) {
        JsonDocument doc;

        doc["system"] = "vesc";
        doc["type"] = "setup_values";
        doc["timestamp_controller"] = millis();

        JsonObject data = doc["data"].to<JsonObject>();
        data["tempMosfet"] = vesc.valuesSetup.tempMosfet;
        data["tempMotor"] = vesc.valuesSetup.tempMotor;
        data["motorCurrent"] = vesc.valuesSetup.motorCurrent;
        data["inputCurrent"] = vesc.valuesSetup.inputCurrent;
        data["dutyCycleNow"] = vesc.valuesSetup.dutyCycleNow;
        data["rpm"] = vesc.valuesSetup.rpm;
        data["speed"] = vesc.valuesSetup.speed;
        data["inpVoltage"] = vesc.valuesSetup.inpVoltage;
        data["batteryLevel"] = vesc.valuesSetup.batteryLevel;
        data["ampHours"] = vesc.valuesSetup.ampHours;
        data["ampHoursCharged"] = vesc.valuesSetup.ampHoursCharged;
        data["wattHours"] = vesc.valuesSetup.wattHours;
        data["wattHoursCharged"] = vesc.valuesSetup.wattHoursCharged;
        data["distance"] = vesc.valuesSetup.distance;
        data["distanceAbs"] = vesc.valuesSetup.distanceAbs;
        data["pidPos"] = vesc.valuesSetup.pidPos;
        data["error"] = vesc.valuesSetup.error;
        data["id"] = vesc.valuesSetup.id;
        data["numVescs"] = vesc.valuesSetup.numVescs;
        data["wattHoursLeft"] = vesc.valuesSetup.wattHoursLeft;
        data["odometer"] = vesc.valuesSetup.odometer;
        data["uptimeMs"] = vesc.valuesSetup.uptimeMs;

        serializeJson(doc, Serial);
        Serial.print('\n');
    }

    if (vesc.getDecodedAdcValues()) {
        JsonDocument doc;
        doc["system"] = "vesc";
        doc["type"] = "decoded_adc_values";
        doc["timestamp_controller"] = millis();

        JsonObject data = doc["data"].to<JsonObject>();
        data["decodedLevel"] = vesc.decodedAdc.decodedLevel;
        data["voltage"] = vesc.decodedAdc.voltage;
        data["decodedLevel2"] = vesc.decodedAdc.decodedLevel2;
        data["voltage2"] = vesc.decodedAdc.voltage2;

        serializeJson(doc, Serial);
        Serial.print('\n');

        handleInputs(vesc.decodedAdc.decodedLevel / 1000000.0, vesc.decodedAdc.decodedLevel2 / 1000000.0);
    }
}
