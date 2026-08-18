#pragma once

#include <Arduino.h>
#include "ClockService.h"
#include "SensorService.h"
#include "LightingService.h"
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
    VictronService victronService;
    MqttService mqttService;

    unsigned long lastSensorScanMs;
    unsigned long lastVictronScanMs;

    void connectToWifi();
    void publishSensorData();
    void publishVictronData();
    void publishDiscovery();
};
