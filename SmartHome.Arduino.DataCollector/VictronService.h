#pragma once

#include <Arduino.h>
#include "VictronParser.h"
#include "ClockService.h"

class VictronService {
public:
    explicit VictronService(ClockService& clock);
    bool begin();
    void update();

    float pvVoltage() const;
    float totalEnergy() const;

private:
    ClockService& clockService;
    HardwareSerial& serial;
    float pvVoltageValue;
    float totalEnergyValue;
    unsigned long lastReadMs;
};
