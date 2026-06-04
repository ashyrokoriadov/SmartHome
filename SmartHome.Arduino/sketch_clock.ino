#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);

  if (!rtc.begin()) {
    Serial.println("Nie znaleziono DS3231!");
    while (1);
  }

  // Ustawienie czasu na datę i godzinę kompilacji
  //rtc.adjust(DateTime(2026, 5, 30, 8, 59, 0));

  DateTime now = rtc.now();

  Serial.print(now.day());
  Serial.print(".");
  Serial.print(now.month());
  Serial.print(".");
  Serial.print(now.year());

  Serial.print(" ");

  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
}

void loop() {
}