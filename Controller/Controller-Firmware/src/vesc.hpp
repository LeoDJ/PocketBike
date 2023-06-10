#include <VescUart.h>

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
        printf("vesc_setup_values;"
            "tempMosfet=%.6g;"
            "tempMotor=%.6g;"
            "motorCurrent=%.6g;"
            "inputCurrent=%.6g;"
            "dutyCycleNow=%.6g;"
            "rpm=%.6g;"
            "speed=%.6g;"
            "inpVoltage=%.6g;"
            "batteryLevel=%.6g;"
            "ampHours=%.6g;"
            "ampHoursCharged=%.6g;"
            "wattHours=%.6g;"
            "wattHoursCharged=%.6g;"
            "distance=%.6g;"
            "distanceAbs=%.6g;"
            "pidPos=%.6g;"
            "error=%d;"
            "id=%d;"
            "numVescs=%d;"
            "wattHoursLeft=%.6g;"
            "odometer=%ld;"
            "uptimeMs=%ld;\n",
            vesc.valuesSetup.tempMosfet,
            vesc.valuesSetup.tempMotor,
            vesc.valuesSetup.motorCurrent,
            vesc.valuesSetup.inputCurrent,
            vesc.valuesSetup.dutyCycleNow,
            vesc.valuesSetup.rpm,
            vesc.valuesSetup.speed,
            vesc.valuesSetup.inpVoltage,
            vesc.valuesSetup.batteryLevel,
            vesc.valuesSetup.ampHours,
            vesc.valuesSetup.ampHoursCharged,
            vesc.valuesSetup.wattHours,
            vesc.valuesSetup.wattHoursCharged,
            vesc.valuesSetup.distance,
            vesc.valuesSetup.distanceAbs,
            vesc.valuesSetup.pidPos,
            vesc.valuesSetup.error,
            vesc.valuesSetup.id,
            vesc.valuesSetup.numVescs,
            vesc.valuesSetup.wattHoursLeft,
            vesc.valuesSetup.odometer,
            vesc.valuesSetup.uptimeMs
        );
    }

    if (vesc.getDecodedAdcValues()) {
        printf("vesc_decoded_adc_values;"
            "decodedLevel=%ld;"
            "voltage=%ld;"
            "decodedLevel2=%ld;"
            "voltage2=%ld;\n",
            vesc.decodedAdc.decodedLevel,
            vesc.decodedAdc.voltage,
            vesc.decodedAdc.decodedLevel2,
            vesc.decodedAdc.voltage2
        );

        handleInputs(vesc.decodedAdc.decodedLevel / 1000000.0, vesc.decodedAdc.decodedLevel2 / 1000000.0);
    }

    // if (vesc.getVescValues()) {
    //     // vesc.printVescValues();
    //     printf("vesc_raw_values;avgMotorCurrent=%.6g;avgInputCurrent=%.6g;dutyCycleNow=%.6g;rpm=%.6g;inpVoltage=%.6g;ampHours=%.6g;ampHoursCharged=%.6g;wattHours=%.6g;wattHoursCharged=%.6g;tachometer=%ld;tachometerAbs=%ld;tempMosfet=%.6g;tempMotor=%.6g;pidPos=%.6g;id=%d;error=%d;\n",
    //         vesc.data.avgMotorCurrent,
    //         vesc.data.avgInputCurrent,
    //         vesc.data.dutyCycleNow,
    //         vesc.data.rpm,
    //         vesc.data.inpVoltage,
    //         vesc.data.ampHours,
    //         vesc.data.ampHoursCharged,
    //         vesc.data.wattHours,
    //         vesc.data.wattHoursCharged,
    //         vesc.data.tachometer,
    //         vesc.data.tachometerAbs,
    //         vesc.data.tempMosfet,
    //         vesc.data.tempMotor,
    //         vesc.data.pidPos,
    //         vesc.data.id,
    //         vesc.data.error
    //     );
    //     // TODO: decide where to do the data processing and send respective values
    // }
}
