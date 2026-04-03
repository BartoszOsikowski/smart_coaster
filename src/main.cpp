#include <Arduino.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 20;

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(663.f);
  scale.tare();
}

void loop() {

  Serial.print("Surowe dane: ");
  Serial.println(scale.get_units(10), 1);

  delay(1500);
  
}