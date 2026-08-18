#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct SensorReading {
    const char* uniqueId;
    const char* name;
    const char* stateTopic;
    const char* unitOfMeasurement;
    const char* deviceClass;
    float value;
};

class HomeAssistantPayloadBuilder {
public:
    static String build(const SensorReading& reading)
    {
        StaticJsonDocument<256> doc;
        doc["state"] = reading.value;

        JsonObject attributes = doc.createNestedObject("attributes");
        attributes["friendly_name"] = reading.name;
        attributes["unique_id"] = reading.uniqueId;
        attributes["state_topic"] = reading.stateTopic;
        attributes["device_class"] = reading.deviceClass;
        attributes["unit_of_measurement"] = reading.unitOfMeasurement;
        attributes["source"] = "SmartHome.Arduino.DataCollector";
        attributes["location"] = DEVICE_LOCATION;

        String json;
        serializeJson(doc, json);
        return json;
    }
};
