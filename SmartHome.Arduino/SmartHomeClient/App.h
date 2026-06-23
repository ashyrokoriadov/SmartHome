#pragma once

#include "ApiClient.h"
#include "ClockService.h"
#include "SensorService.h"
#include "MatrixDisplay.h"
#include "LightingService.h"

class App {
public:
    void setup();
    void loop();

private:
    ApiClient apiClient;
    ClockService clockService;
    SensorService sensorService;
    MatrixDisplay display;
    LightingService lightingService;

    unsigned long lastUpdate = 0;
    unsigned long updateInterval = 120000;

    void connectWiFi();
    void sendMeasurements();
};