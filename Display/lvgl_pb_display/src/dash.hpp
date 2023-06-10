#include "widgets/vertical_bar.h"
#include "widgets/numericalValue.h"
#include "widgets/gauge.h"
#include "widgets/graph.h"

VerticalBar *bat, *tempBar;
NumericalValue *batVolt, *batAmp, *escTemp, *motTemp, *mileage, *distance, *range, *topSpd;
Gauge *speed, *rpm;
Graph *graph;

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

    rpm = new Gauge(cont, 16, 0, 4, 4);
    rpm->init(0, 1000, 200, 100, 800, "RPM", "%.0f");
    rpm->draw();
    rpm->setValue(570);

    graph = new Graph(cont, 8, 0, 8, 2);
    graph->init(60, 
        {.minVal = -60,  .maxVal = 0,   .majorIncrementValue = 10, .minorIncrementNumber = 3, .axisUnitText = "s"}, 
        {.minVal = -100, .maxVal = 100, .majorIncrementValue = 50, .minorIncrementNumber = 1, .axisUnitText = "A"}, 
        {.minVal = 0,    .maxVal = 50,  .majorIncrementValue = 25, .minorIncrementNumber = 1, .axisUnitText = "km/h"});
    graph->draw();

    for (int i = 0; i < 60; i++) {
        graph->setValue(i*i/32-50, i*i/32);
    }

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
    topSpd->setValue(66);
}

void dashUpdate(float batPercent_, float escTemp_, float batVolt_, float batAmp_, float speed_, float rpm_, float motTemp_, float distance_,    float motAmp_, float wh_) {
    bat->setValue(batPercent_ * 100.0);
    tempBar->setValue(escTemp_);
    escTemp->setValue(escTemp_);
    batVolt->setValue(batVolt_);
    batAmp->setValue(batAmp_);
    speed->setValue(speed_ * 3.6);
    rpm->setValue(rpm_);
    graph->setValue(motAmp_, speed_ * 3.6);
    motTemp->setValue(motTemp_);
    mileage->setValue((wh_ / 1000.0) / (distance_ / 100000.0));    // TODO: average
    distance->setValue(distance_ / 1000.0);
    topSpd->setValue(0);    // TODO
}