#pragma once

#include "SensorService.h"
#include "ClockService.h"

class LightingService {
public:
    void toggleLightIfPossible();

private:
    ClockService clockService;
    SensorService sensorService;
};