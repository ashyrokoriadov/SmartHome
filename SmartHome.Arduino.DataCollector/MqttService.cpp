#include "MqttService.h"
#include "Config.h"

bool MqttService::begin(const char* brokerHost, uint16_t port)
{
    connected = false;
    client.setClient(wifiClient);
    client.setServer(brokerHost, port);

    if (!client.connect("SmartHomeCollector")) {
        Serial.print("MQTT connection failed, state = ");
        Serial.println(client.state());
        return false;
    }

    connected = true;
    Serial.println("MQTT connected.");
    return true;
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
        if (!client.connect("SmartHomeCollector")) {
            Serial.print("MQTT reconnect failed, state = ");
            Serial.println(client.state());
            connected = false;
        } else {
            connected = true;
            Serial.println("MQTT reconnected.");
        }
    }
}
