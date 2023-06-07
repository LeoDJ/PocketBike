#include <VescUart.h>

VescUart vesc;

// just copy the struct for now, as it's private in the library :/ (TODO: solve more cleanly)
typedef struct McConfPackage {
    float currentMin;
    float currentMax;
    float erpmMin;
    float erpmMax;
    float dutyMin;
    float dutyMax;
    float wattMin;
    float wattMax;
    float inCurrentMin;
    float inCurrentMax;
    int   motorPoles;
    float gearRatio;
    float wheelDiameter;
} vescMcConf_t;

vescMcConf_t vescMcConf = {
    .motorPoles = -1
};


void vescGetMcConf() {
    if (vesc.getMcConfValues()) {
        memcpy(&vescMcConf, &vesc.mcconf, sizeof(vescMcConf)); // evil, but just a hack for now
    }
}

void vescPoll() {
    if (vescMcConf.motorPoles == -1) {
        vescGetMcConf();
    }

    // if (vesc.getMcConfValues()) {
    //     printf("vesc_raw_mcconf;ICMin=%.6g;ICMax=%.6g;MP=%d;GR=%.6g;WD=%.6g\n", 
    //         vesc.mcconf.inCurrentMin,
    //         vesc.mcconf.inCurrentMax,
    //         vesc.mcconf.motorPoles,
    //         vesc.mcconf.gearRatio,
    //         vesc.mcconf.wheelDiameter
    //         );
    // }

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

        


        // probably don't need that if I implement COMM_GET_VALUES_SETUP, but for now I'll do it like this (TODO)
        float speedFact = ((vescMcConf.motorPoles / 2.0) * 60.0 * vescMcConf.gearRatio) / (vescMcConf.wheelDiameter * PI); // (gear ratio = wheel/motor)
        if (speedFact < 1e-3) {
            speedFact = 1e-3;
        }
        float speed = vesc.data.rpm / speedFact * 3.6; // idk what comes out, mm/s, m/s, km/h?!
        float dist = vesc.data.tachometerAbs / speedFact; // idk
        float whConsumed = vesc.data.wattHours - vesc.data.wattHoursCharged;
        float whKmTotal = whConsumed / max(dist, 1e-10);
        float power = vesc.data.inpVoltage * vesc.data.avgInputCurrent;

        printf("vesc_calc_values;speed=%.6g;dist=%.6g;whConsumed=%.6g;whKmTotal=%.6g;power=%.6g\n",
            speed,
            dist,
            whConsumed,
            whKmTotal,
            power
        );
    }
}
