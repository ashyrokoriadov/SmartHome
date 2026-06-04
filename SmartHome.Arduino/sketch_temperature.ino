#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);

  if (!ina219.begin()) {
    Serial.println("Nie znaleziono INA219!");
    while (1);
  }
}

void loop() {
  float voltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  float power = voltage * current / 1000.0;

  Serial.print("Napiecie: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Prad: ");
  Serial.print(current);
  Serial.println(" mA");

  Serial.print("Moc: ");
  Serial.print(power);
  Serial.println(" W");

  delay(1000);
}