#include "MqttService.h"
#include "Config.h"

bool MqttService::begin(const char* brokerHost, uint16_t port)
{
    connected = false;
    client.setClient(wifiClient);
    client.setBufferSize(4096);
    client.setServer(brokerHost, port);

    Serial.println("MQTT C: connecting...");

    if (!client.connect(MQTT_CLIENT_ID)) {
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
    const bool isConnected = client.connected();

    if (!isConnected) {
        Serial.print("MQTT publish SKIPPED: not connected, topic=");
        Serial.println(topic);
        return false;
    }

    const bool published = client.publish(
        topic.c_str(),
        payload.c_str(),
        true
    );

    Serial.print("MQTT publish: topic=");
    Serial.print(topic);
    Serial.print(" connected=");
    Serial.print(isConnected);
    Serial.print(" result=");
    Serial.println(published ? "OK" : "FAILED");

    if (!published) {
        Serial.print("MQTT publish failed, client state=");
        Serial.println(client.state());
    }

    return published;
}

void MqttService::reconnectIfNeeded()
{
    if (!client.connected()) {
        Serial.print("MQTT reconnect attempt. state = ");
        Serial.println(client.state());

        Serial.println("MQTT R: reconnecting...");
        
        if (!client.connect(MQTT_CLIENT_ID)) {
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
