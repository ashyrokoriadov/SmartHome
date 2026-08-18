#pragma once

#include <Arduino.h>
#include "ClockService.h"
#include "SensorService.h"

class LightingService {
public:
    LightingService(ClockService& clock, SensorService& sensorService);
    void update();

private:
    ClockService& clockService;
    SensorService& sensorService;
    bool timeCondition;
    bool lightCondition;
    bool changed;
    bool turnedOn;
};
