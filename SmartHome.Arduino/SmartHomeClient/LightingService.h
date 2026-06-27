#pragma once

#include "SensorService.h"
#include "ClockService.h"

class LightingService {
public:
    LightingService(ClockService& clock);
    void toggleLightIfPossible();

private:
    ClockService& clockService;
    SensorService sensorService;

    bool timeCondition;
    bool lightCondition;
    bool changed;
    bool turnedOn;
};