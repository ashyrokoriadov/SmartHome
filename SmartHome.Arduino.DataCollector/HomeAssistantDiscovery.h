#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Config.h"

class HomeAssistantDiscovery {
public:
	static String buildSensorConfig(const char* uniqueId, const char* name, const char* stateTopic, const char* unit, const char* deviceClass)
	{
		StaticJsonDocument<512> doc;
		doc["name"] = name;
		doc["unique_id"] = uniqueId;
		doc["state_topic"] = stateTopic;
		if (unit && unit[0] != '\0') {
			doc["unit_of_measurement"] = unit;
		}
		if (deviceClass && deviceClass[0] != '\0') {
			doc["device_class"] = deviceClass;
		}
		doc["value_template"] = "{{ value_json.state }}";

		JsonObject device = doc.createNestedObject("device");
		device["identifiers"] = String("smarthome_collector_") + DEVICE_LOCATION;
		device["name"] = String("SmartHome Collector ") + DEVICE_LOCATION;
		device["model"] = "SmartHome Collector";
		device["manufacturer"] = "DIY";		

		String json;
		serializeJson(doc, json);
		return json;
	}
};
