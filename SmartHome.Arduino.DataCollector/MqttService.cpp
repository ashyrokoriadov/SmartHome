#include "MqttService.h"
#include "Config.h"

bool MqttService::begin(const char* brokerHost, uint16_t port)
{
    connected = false;
    client.setClient(wifiClient);
    client.setServer(brokerHost, port);

    // quick TCP check to broker before attempting MQTT connect
    {
        WiFiClient testClient;
        Serial.println("Testing TCP connection to MQTT...");
        if (testClient.connect(brokerHost, port)) {
            Serial.println("TCP connection SUCCESS");
            testClient.stop();
        } else {
            Serial.println("TCP connection FAILED");
        }
    }

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
        // quick TCP check to broker before attempting MQTT reconnect
        {
            WiFiClient testClient;
            Serial.println("Testing TCP connection to MQTT...");
            if (testClient.connect(MQTT_BROKER, MQTT_PORT)) {
                Serial.println("TCP connection SUCCESS");
                testClient.stop();
            } else {
                Serial.println("TCP connection FAILED");
            }
        }

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
