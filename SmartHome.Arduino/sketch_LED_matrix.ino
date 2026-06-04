#include "ArduinoGraphics.h"
#include <Arduino_LED_Matrix.h>
#include <TextAnimation.h>
#include <gallery.h>


ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();
}

void loop() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFF);
  matrix.textScrollSpeed(160);

  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print("ERROR ABC");
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();

  delay(1000);
}