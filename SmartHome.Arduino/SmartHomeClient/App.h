#pragma once

#include "ApiClient.h"
#include "ClockService.h"
#include "SensorService.h"
#include "MatrixDisplay.h"
#include "LightingService.h"
#include "VictronParser.h"

class App {
public:
    App(ClockService& clock);

    void setup();
    void loop();

private:
    ApiClient apiClient;
    ClockService& clockService;   // reference, not copy
    SensorService sensorService;
    MatrixDisplay display;
    LightingService lightingService;

    unsigned long lastUpdate = 0;
    unsigned long lastSend = 0;
    unsigned long updateInterval = 120000;   

    void connectWiFi();
    void sendMeasurements();
    void postParsedMeasurement(const ParsedValue& measurement);
};