#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <U8g2lib.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 1;
const int LOADCELL_SCK_PIN = 0;
unsigned long rememberedTime = 0;
unsigned long rememberedRfidTime = 0;
HX711 scale;
Adafruit_PN532 nfc(-1, -1);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2);

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);
  nfc.begin();
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-638.0f);
  scale.tare();

  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  
  
}

void loop() {
  unsigned long currentTime = millis();
  //-------buttons
  if(!digitalRead(20)){
    Serial.println("Button 1 pressed");
  }
  if(!digitalRead(21)){
    Serial.println("Button 2 pressed");
  }
  

  //-------------- NFC
  if(currentTime - rememberedRfidTime >= 250) {
    rememberedRfidTime = currentTime; 

    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; 
    uint8_t uidLength;   
    
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if(success){
      Serial.print("UID value: ");
      nfc.PrintHex(uid, uidLength);
    }
  }
  if(currentTime - rememberedTime > 1000) {
    rememberedTime = currentTime;
    //--------------- HX711
    float weight = scale.get_units(1);
    Serial.println("Readings from HX711:");
    Serial.println(weight);
    //-------display
    u8g2.clearBuffer();
    u8g2.setCursor(0,25);
    u8g2.print(weight, 0);
    u8g2.sendBuffer();
    
  }
}
