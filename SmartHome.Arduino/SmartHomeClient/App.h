#pragma once

#include "ApiClient.h"
#include "ClockService.h"
#include "SensorService.h"
#include "MatrixDisplay.h"

class App {
public:
    void setup();
    void loop();

private:
    ApiClient apiClient;
    ClockService clockService;
    SensorService sensorService;
    MatrixDisplay display;

    unsigned long lastUpdate = 0;
    unsigned long updateInterval = 120000;

    void connectWiFi();
    void sendMeasurements();
};