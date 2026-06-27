#include <Arduino.h>
#include <WiFiS3.h>

#include "App.h"
#include "Config.h"
#include <ArduinoJson.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include "LightingService.h"

App::App(ClockService& clock)
    : clockService(clock),
      lightingService(clock)
{}

void App::setup()
{
    unsigned long start = millis();

    Serial.begin(115200);
    while (!Serial && millis() - start < 3000);
    
    display.begin(); 

    connectWiFi();

    sensorService.begin();
    clockService.begin();

    pinMode(LAMPS_CONTROL_PIN, OUTPUT);

    updateInterval = API_REQUEST_INTERVAL;
}

void App::loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connectWiFi();
    }   

    //lightingService.toggleLightIfPossible(); 

    if (millis() - lastSend >= updateInterval)
    {
        lastSend = millis();
        clockService.syncIfNeeded(apiClient);  
        sendMeasurements();
    }
}

void App::connectWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    Serial.print("Connecting to WiFi");

    IPAddress ip(192, 168, 1, 217);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress primaryDNS(8, 8, 8, 8);

    WiFi.config(ip, gateway, subnet, primaryDNS);

    while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED)
    {
        Serial.print(".");
        delay(2000);
    }

    Serial.println();
    Serial.println("WiFi connected");

    display.printScroll("WiFi OK");
}

void App::sendMeasurements()
{
    SensorData data = sensorService.readAll();

    char timestamp[32];
    clockService.formatUtc(timestamp, sizeof(timestamp));

    bool isHealthy = apiClient.healthCheck();
    if (!isHealthy)
    {
        display.printStatic("E0");
        updateInterval = API_REQUEST_FAIL_WAIT_TIME; 
        Serial.println("Unhealthy.");
        return;
    }
    else
    {
        updateInterval = API_REQUEST_INTERVAL; 
        Serial.println("Healthy.");
    }

    char correlationId[128] = "";
    if (!apiClient.get(
            "/MetaData/CorrelationId",
            correlationId,
            sizeof(correlationId)))
    {
        Serial.println("Failed to fetch correlation id");
        display.printStatic("E1");
        return;
    }

    bool temperaturePostResult = false;
    bool lightPostResult = false;
    bool electricalPostResult = false;

    // Temperature payload
    {
        StaticJsonDocument<512> doc;
        char jsonBuffer[512];

        doc["correlationId"] = correlationId;
        doc["timestamp"] = timestamp;
        doc["location"] = LOCATION;
        doc["temperature"] = data.temperature;

        serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));  
        
        temperaturePostResult = apiClient.post("/Temperature/Add", jsonBuffer);    
    }

    // Light sensor payload
    {
        StaticJsonDocument<512> doc;
        char jsonBuffer[512];

        doc["correlationId"] = correlationId;
        doc["timestamp"] = timestamp;
        doc["location"] = LOCATION;
        doc["analogueValue"] = data.lightAnalog;
        doc["digitalValue"] = data.lightDigital;

        serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

        lightPostResult = apiClient.post("/LightSensor/Add", jsonBuffer);
    }

    // Battery payload
    {
        StaticJsonDocument<512> doc;
        char jsonBuffer[512];

        doc["correlationId"] = correlationId;
        doc["timestamp"] = timestamp;
        doc["location"] = LOCATION;
        doc["Voltage"] = data.voltage;
        doc["Current"] = data.current;
        doc["PanelVoltage"] = data.panelVoltage;
        doc["PanelPower"] = data.panelPower;
        doc["ChargerState"] = data.chargerState;
        doc["Error"] = data.error;
        doc["GainedEnergyToday"] = data.gainedEnergyToday;

        serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

        electricalPostResult = apiClient.post("/Battery/Add", jsonBuffer);
    }

    if (temperaturePostResult & lightPostResult & electricalPostResult)
    {
        display.printStatic("OK");
    }
    else
    {
        if (!temperaturePostResult)
        {
            display.printStatic("TF");
        }

        if (!lightPostResult)
        {
            display.printStatic("LF");
        }

        if (!electricalPostResult)
        {
            display.printStatic("EF");
        }
    }    
}