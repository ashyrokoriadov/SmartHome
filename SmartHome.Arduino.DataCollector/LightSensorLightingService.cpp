#include "LightSensorLightingService.h"
#include "Config.h"

LightSensorLightingService::LightSensorLightingService(SensorService& sensor)
    : sensorService(sensor),
      lightCondition(false),
      changed(false),
      turnedOn(false)
{}

void LightSensorLightingService::update()
{
    const bool newLightCondition =
        sensorService.readLightDigital() == LIGHT_SENSOR_ON_DIGITAL_VALUE;

    changed = (lightCondition != newLightCondition);
    lightCondition = newLightCondition;
    turnedOn = newLightCondition;

    digitalWrite(LAMPS_CONTROL_PIN, turnedOn ? HIGH : LOW);

    if (changed)
    {
        Serial.print("Lamps = ");
        Serial.print(turnedOn ? "ON" : "OFF");
        Serial.print("; lightSensor=");
        Serial.println(newLightCondition ? "true" : "false");
    }
}
