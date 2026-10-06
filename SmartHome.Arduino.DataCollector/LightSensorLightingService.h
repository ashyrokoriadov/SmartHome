#pragma once

#include "SensorService.h"

class LightSensorLightingService
{
public:
    explicit LightSensorLightingService(SensorService& sensor);

    void update();

private:
    SensorService& sensorService;

    bool lightCondition;
    bool changed;
    bool turnedOn;
};
