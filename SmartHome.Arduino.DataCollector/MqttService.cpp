#include "MqttService.h"
#include "Config.h"

bool MqttService::begin(const char* brokerHost, uint16_t port)
{
    connected = false;
    client.setClient(wifiClient);
    client.setBufferSize(4096);
    client.setServer(brokerHost, port);

    if (!client.connect("SmartHomeCollector C")) {
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
        Serial.print("MQTT reconnect attempt. state = ");
        Serial.println(client.state());

        if (!client.connect("SmartHomeCollector R")) {
            Serial.print("MQTT reconnect failed, state = ");
            Serial.println(client.state());
            connected = false;
        }
        else {
            connected = true;
            Serial.println("MQTT reconnected.");
        }
    }
}

void MqttService::loop()
{
    static unsigned long lastLoopMs = millis();

    unsigned long now = millis();
    unsigned long gap = now - lastLoopMs;
    lastLoopMs = now;

    if (gap > 1000) {
        Serial.print("WARNING: MQTT loop gap = ");
        Serial.print(gap);
        Serial.println(" ms");
    }

    client.loop();
    reconnectIfNeeded();    
}
