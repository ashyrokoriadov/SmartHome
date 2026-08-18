#include "MqttService.h"
#include "Config.h"

bool MqttService::begin(const char* brokerHost, uint16_t port)
{
    connected = false;
    client.setClient(wifiClient);
    client.setServer(brokerHost, port);

    if (client.connect("SmartHomeCollector")) {
        connected = true;
        Serial.println("MQTT connected.");
        return true;
    }

    Serial.println("MQTT connection failed.");
    return false;
}

bool MqttService::publishJson(const String& topic, const String& payload)
{
    if (!client.connected()) {
        return false;
    }

    bool published = client.publish(topic.c_str(), payload.c_str(), true);
    if (!published) {
        Serial.print("MQTT publish failed for topic: ");
        Serial.println(topic);
    }

    return published;
}

void MqttService::reconnectIfNeeded()
{
    if (!client.connected()) {
        Serial.println("MQTT reconnect attempt.");
        connected = client.connect("SmartHomeCollector");
        if (!connected) {
            Serial.println("MQTT reconnect failed.");
        }
    }
}
