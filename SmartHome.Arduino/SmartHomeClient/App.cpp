#include <Arduino.h>
#include <WiFiS3.h>

#include "App.h"
#include "Config.h"
#include <ArduinoJson.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include "LightingService.h"
#include "SoftwareSerial.h"
#include "VictronParser.h"

App::App(ClockService& clock)
    : clockService(clock),
      lightingService(clock)
{}

SoftwareSerial victron(VICTRON_RX_PIN, VICTRON_TX_PIN);
VictronSerialReader victronSerialReader;

void App::setup()
{
    unsigned long start = millis();

    Serial.begin(115200);
    while (!Serial && millis() - start < 3000);
    
    //display.begin(); 

    connectWiFi();

    sensorService.begin();
    clockService.begin();

    pinMode(LAMPS_CONTROL_PIN, OUTPUT);
    pinMode(VICTRON_RX_PIN, INPUT);

    victron.begin(19200);

    updateInterval = API_REQUEST_INTERVAL;
}

void App::loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connectWiFi();
    }   

    lightingService.toggleLightIfPossible(); 

    if (millis() - lastSend >= updateInterval)
    {
        Serial.println(digitalRead(8));
        
        lastSend = millis();
        clockService.syncIfNeeded(apiClient);  
        sendMeasurements();
    }

    int counter = 0;

    while (victron.available())
    {
        byte b = victron.read();
        String token = (b == 'D') ? "D" : String(b, HEX);
        ParsedValue parsedValue = victronSerialReader.ReadByte(token);

        if (!parsedValue.IsEmpty() && parsedValue.Name != "Unknown")
        {
            //Serial.print("Victron ");
            //Serial.print(parsedValue.Name);
            //Serial.print(": ");
            //Serial.println(parsedValue.Value);

            counter++;

            postParsedMeasurement(parsedValue);
        }

        if (counter == 8)
        {
            break;
        }
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

void App::postParsedMeasurement(const ParsedValue& measurement)
{
    char timestamp[32];
    clockService.formatUtc(timestamp, sizeof(timestamp));

    char correlationId[128] = "";
    if (!apiClient.get("/MetaData/CorrelationId", correlationId, sizeof(correlationId)))
    {
        Serial.println("Failed to fetch correlation id for parsed measurement");
        return;
    }

    StaticJsonDocument<512> doc;
    char jsonBuffer[512];

    doc["correlationId"] = correlationId;
    doc["timestamp"] = timestamp;
    doc["location"] = LOCATION;
    doc["name"] = measurement.Name;
    doc["value"] = measurement.Value;
    doc["dataType"] = "System.Decimal";

    serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

    bool result = apiClient.post("/BatteryData/Add", jsonBuffer);
    if (!result)
    {
        Serial.print("Failed to post measurement ");
        Serial.println(measurement.Name);
    }
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
        //StaticJsonDocument<512> doc;
        //char jsonBuffer[512]; 
        //serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
        //electricalPostResult = apiClient.post("/Battery/Add", jsonBuffer);
        electricalPostResult = true;
    }

    if (temperaturePostResult & lightPostResult & electricalPostResult)
    {
        display.printStatic("OK");
    }
    else
    {
        String errorMessage = "";

        if (!temperaturePostResult)
        {
            errorMessage += "TF ";
        }

        if (!lightPostResult)
        {
            errorMessage += "LF ";
        }

        if (!electricalPostResult)
        {
            errorMessage += "EF ";
        }

        errorMessage.trim();

        if (errorMessage.length() > 0) 
        {
            display.printScroll(errorMessage.c_str());
            display.printStatic("FAIL");
        }   
    }    
}