#pragma once

#include "ClockService.h"

class TimeScheduleLightingService
{
public:
    explicit TimeScheduleLightingService(ClockService& clock);

    void update();

private:
    ClockService& clockService;

    bool timeCondition;
    bool changed;
    bool turnedOn;

    bool rtcFailureActive;
    unsigned long rtcFailureStart;
};
