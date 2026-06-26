#include "App.h"

ClockService clockService;
App app(clockService);  

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}