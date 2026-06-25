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

    Serial.println("SETUP - Before syncronizing a clock.");
    clockService.syncIfNeeded(apiClient);

    pinMode(LAMPS_CONTROL_PIN, OUTPUT);
}

void App::loop()
{
    Serial.println("LOOP - Starting loop.");

    if (WiFi.status() != WL_CONNECTED)
    {
        connectWiFi();
    }   

    Serial.println("LOOP - Toggling lights.");
    lightingService.toggleLightIfPossible(); 

    Serial.println("LOOP - Evaluating interval length.");

    if (millis() - lastSend >= API_REQUEST_INTERVAL)
    {
        lastSend = millis();

        Serial.println("LOOP - Before syncronizing a clock.");
        clockService.syncIfNeeded(apiClient);        

        Serial.println("LOOP - Before sending measurements.");
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
    Serial.println("Reading measurements...");
    SensorData data = sensorService.readAll();
    Serial.println("Measurements have been read.");

    char timestamp[32];
    clockService.formatUtc(timestamp, sizeof(timestamp));

    Serial.println(timestamp);

    // Health check
    if (!apiClient.healthCheck())
    {
        display.printStatic("E0");
        updateInterval = API_REQUEST_FAIL_WAIT_TIME; 
        return;
    }
    else
    {
        updateInterval = API_REQUEST_INTERVAL; 
    }

    // Fetch correlation id from API
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

    // Temperature payload
    {
        StaticJsonDocument<512> doc;
        char jsonBuffer[512];

        doc["correlationId"] = correlationId;
        doc["timestamp"] = timestamp;
        doc["location"] = LOCATION;
        doc["temperature"] = data.temperature;

        serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

        apiClient.post("/Temperature/Add", jsonBuffer);
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

        apiClient.post("/LightSensor/Add", jsonBuffer);
    }

    // Battery payload
    /*
    {
        StaticJsonDocument<512> doc;
        char jsonBuffer[512];

        doc["correlationId"] = correlationId;
        doc["timestamp"] = timestamp;
        doc["location"] = LOCATION;
        doc["voltage"] = data.voltage;
        doc["current"] = data.current;

        serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

        apiClient.post("/Battery/Add", jsonBuffer);
    }
    */

    display.printStatic("OK");
}