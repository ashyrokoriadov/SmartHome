#pragma once

#include "ClockService.h"
#include "SensorService.h"

class LightingService
{
public:
    LightingService(ClockService& clock, SensorService& sensor);

    void update();

private:
    ClockService& clockService;
    SensorService& sensorService;

    bool timeCondition;
    bool lightCondition;
    bool changed;
    bool turnedOn;

    bool rtcFailureActive;
    unsigned long rtcFailureStart;
};
