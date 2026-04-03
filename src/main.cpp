#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 20;

HX711 scale;
Adafruit_PN532 nfc(-1, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);
  nfc.begin();
  nfc.SAMConfig();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(663.f);
  scale.tare();
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("Karta: ");
    for (int i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
  Serial.print("Surowe dane: ");
  Serial.println(scale.get_units(10), 1);

  delay(1500);
  }
  
}