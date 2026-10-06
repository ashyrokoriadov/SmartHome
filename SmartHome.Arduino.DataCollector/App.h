#pragma once

#include <Arduino.h>
#include "ClockService.h"
#include "SensorService.h"
#include "LightingService.h"
#include "LightSensorLightingService.h"
#include "TimeScheduleLightingService.h"
#include "VictronService.h"
#include "MqttService.h"

class App {
public:
    App();

    void setup();
    void loop();

private:
    ClockService clockService;
    SensorService sensorService;
    LightingService lightingService;
    LightSensorLightingService lightSensorLightingService;
    TimeScheduleLightingService timeScheduleLightingService;
    VictronService victronService;
    MqttService mqttService;

    unsigned long lastSensorScanMs;
    unsigned long lastVictronScanMs;

    bool connectToWifi();
    void publishSensorData();
    void publishVictronData();
    void publishDiscovery();
};
