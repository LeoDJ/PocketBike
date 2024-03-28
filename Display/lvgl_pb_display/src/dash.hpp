#include "widgets/vertical_bar.h"
#include "widgets/numericalValue.h"
#include "widgets/gauge.h"
#include "widgets/graph.h"
#include "util.h"

#include <mutex>
#include <chrono>
#include <ArduinoJson.h>

VerticalBar *bat, *tempBar;
NumericalValue *batVolt, *batAmp, *escTemp, *motTemp, *mileage, *distance, *range, *topSpd;
Gauge *speed, *rpm, *motCurGauge;
Graph *graph;
lv_obj_t *networkText, *additText;

float topSpeedVal = 0;

void dashInit(lv_obj_t *cont) {
    bat = new VerticalBar(cont, 0, 0, 1, 4);
    bat->init("Batt.");
    bat->draw();
    bat->setValue(90);

    // TODO: color
    tempBar = new VerticalBar(cont, 23, 0, 1, 4);
    tempBar->init("ESC", "%2.0f°C");
    tempBar->draw();
    tempBar->setValue(25.9);

    batVolt = new NumericalValue(cont, 1, 0, 3, 1);
    batVolt->init("Batt\nVolt");
    batVolt->draw();
    batVolt->setValue(3.9*12);

    batAmp = new NumericalValue(cont, 1, 1, 3, 1);
    batAmp->init("Batt\nAmps");
    batAmp->draw();
    batAmp->setValue(43);

    speed = new Gauge(cont, 4, 0, 4, 4);
    speed->init(0, 50, 10, 2, 30, "km/h");
    speed->draw();
    speed->setValue(33.3);

    // rpm = new Gauge(cont, 16, 0, 4, 4);
    // rpm->init(0, 1000, 200, 100, 800, "RPM", "%.0f");
    // rpm->draw();
    // rpm->setValue(570);

    motCurGauge = new Gauge(cont, 16, 0, 4, 4);
    motCurGauge->init(-100, 100, 20, 10, 80, "Motor\nA", "%.1f");
    motCurGauge->draw();
    motCurGauge->setValue(570);

    graph = new Graph(cont, 8, 0, 8, 3);
    graph->init(60, 
        {.minVal = -60,  .maxVal = 0,   .majorIncrementValue = 10, .minorIncrementNumber = 3, .axisUnitText = "s"}, 
        {.minVal = -80,  .maxVal = 80,  .majorIncrementValue = 20, .minorIncrementNumber = 1, .axisUnitText = "A"}, 
        {.minVal = 0,    .maxVal = 50,  .majorIncrementValue = 10, .minorIncrementNumber = 1, .axisUnitText = "km/h"});
    graph->draw();

    // for (int i = 0; i < 60; i++) {
    //     graph->setValue(i*i/32-50, i*i/32);
    // }

    escTemp = new NumericalValue(cont, 1, 2, 3, 1);
    escTemp->init("ESC\n°C");
    escTemp->draw();
    escTemp->setValue(25.9);

    motTemp = new NumericalValue(cont, 1, 3, 3, 1);
    motTemp->init("Motor\n°C");
    motTemp->draw();
    motTemp->setValue(60.9);

    mileage = new NumericalValue(cont, 20, 0, 3, 1);
    mileage->init("kWh/\n100km");
    mileage->draw();
    mileage->setValue(5);

    distance = new NumericalValue(cont, 20, 1, 3, 1);
    distance->init("Travel.\nkm", "%5.2f");
    distance->draw();
    distance->setValue(13.37);

    range = new NumericalValue(cont, 20, 2, 3, 1);
    range->init("Est.\nkm", "%5.2f");
    range->draw();
    range->setValue(42.23);

    topSpd = new NumericalValue(cont, 20, 3, 3, 1);
    topSpd->init("Max\nkm/h");
    topSpd->draw();
    topSpd->setValue(0);


    lv_obj_t * networkTextContainer = lv_obj_create(cont);
    lv_obj_set_grid_cell(networkTextContainer, LV_GRID_ALIGN_STRETCH, 8, 8, LV_GRID_ALIGN_STRETCH, 3, 1);
    lv_obj_set_style_pad_all(networkTextContainer, 1, LV_PART_MAIN);                   // reduce padding
    lv_obj_set_style_opa(networkTextContainer, LV_OPA_TRANSP, LV_PART_SCROLLBAR);      // hide scrollbars

    networkText = lv_label_create(networkTextContainer);
    lv_obj_set_style_text_font(networkText, lv_theme_default_get()->font_small, LV_PART_MAIN);
    lv_label_set_text(networkText, "SSID: \nIP: \nMQTT queued msgs: ");
    lv_obj_set_width(networkText, LV_PCT(66));
    lv_obj_align(networkText, LV_ALIGN_LEFT_MID, 0, 0);
    // lv_obj_align(networkText, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    additText = lv_label_create(networkTextContainer);
    lv_obj_set_style_text_font(additText, lv_theme_default_get()->font_small, LV_PART_MAIN);
    lv_label_set_text(additText, "GPS Speed: \nGPS Fix: ");
    lv_obj_set_width(additText, LV_PCT(33));
    lv_obj_align(additText, LV_ALIGN_RIGHT_MID, 0, 0);

}

time_t lastGraphUpdate = 0;

void dashUpdate(float batPercent_, float escTemp_, float batVolt_, float batAmp_, float speed_, float rpm_, float motTemp_, float distance_,    float motAmp_, float wh_) {
    bat->setValue(batPercent_ * 100.0);
    tempBar->setValue(escTemp_);
    escTemp->setValue(escTemp_);
    batVolt->setValue(batVolt_);
    batAmp->setValue(batAmp_);
    speed->setValue(speed_ * 3.6);
    // rpm->setValue(rpm_);
    motCurGauge->setValue(motAmp_);
    motTemp->setValue(motTemp_);
    mileage->setValue((wh_ / 1000.0) / (distance_ / 100000.0));    // TODO: average
    distance->setValue(distance_ / 1000.0);
    range->setValue(0);     // TODO
    if (speed_ > topSpeedVal) {
        topSpeedVal = speed_;
        topSpd->setValue(topSpeedVal);
    }

    // TODO: handle better
    int now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (now - lastGraphUpdate >= 1) {
        lastGraphUpdate = now;
        graph->setValue(motAmp_, speed_ * 3.6);
    }

    std::lock_guard<std::mutex> lock(mutexIpSsid);
    lv_label_set_text_fmt(networkText, "SSID: %s\nIP: %s\nMQTT queued msgs: %lu", curSsidStr.c_str(), curIpStr.c_str(), curMqttQueued);

}

float gpsSpeed = 0, gpsAcc = -1;
int gpsFix = 1;

void dashUpdate(JsonDocument doc) {
    std::string system = doc["system"];
    std::string type = doc["type"];
    if (system == "gps") {
        if (type == "GSA") {
            gpsFix = doc["data"]["fixType"];
        }
        else if (type == "RMC") {
            gpsSpeed = doc["data"]["speed"];
        }
        else if (type == "GST") {
            gpsAcc = doc["data"]["acc_m"];
        }
        lv_label_set_text_fmt(additText, "GPS Speed: %5.1f\nGPS Fix: %s\nGPS Acc: %5.1fm", gpsSpeed, Util::gpsFix2Str(gpsFix), gpsAcc);
    }

}