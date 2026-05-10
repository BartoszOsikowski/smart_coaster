#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <U8g2lib.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 20;

HX711 scale;
Adafruit_PN532 nfc(-1, -1);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);
  //-------------------------
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0, 20, "Start...");
  u8g2.sendBuffer();
  //----------------------
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