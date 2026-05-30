#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <U8g2lib.h>
#include <Preferences.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 1;
const int LOADCELL_SCK_PIN = 0;
unsigned long rememberedTime = 0;
unsigned long rememberedRfidTime = 0;
unsigned long rememberedButtonTime1 = 0;
unsigned long rememberedButtonTime2 = 0;
bool lastButtonState1 = HIGH; 
bool lastButtonState2 = HIGH;
int currentDisplay = 0;
int currentOption = 0;
int dailyFluid = 0; 
int dailyFluidSet = 1000; 

String UID = "";
float savedWeight;
String savedUID;
bool isOnPad = false;
float lastLiquidAmount = 0.0;

HX711 scale;
Adafruit_PN532 nfc(-1, -1);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2);
Preferences preferences;

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);
  nfc.begin();
  u8g2.begin();
  u8g2.setFont(u8g2_font_profont12_tr);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-638.0f);
  scale.tare();

  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);

  preferences.begin("settings", false); 
  savedUID = preferences.getString("uidRFID", "");
  savedWeight = preferences.getFloat("weight", 0.0);
}

void loop() {
  unsigned long currentTime = millis();
  
  //-------buttons
  bool buttonState1 = digitalRead(20);
  bool buttonState2 = digitalRead(21);
  
  
  if(buttonState1 == LOW && currentTime - rememberedButtonTime1 > 200 && lastButtonState1 == HIGH){
    rememberedButtonTime1 = currentTime;
    currentDisplay++;
    if(currentDisplay > 1){ 
      currentDisplay = 0;
    }
    currentOption = 0;
    rememberedTime = 0;
  }
  lastButtonState1 = buttonState1;
  
  
  if(buttonState2 == LOW && currentTime - rememberedButtonTime2 > 200 && lastButtonState2 == HIGH){
    rememberedButtonTime2 = currentTime;
    if(currentDisplay == 1){
      currentOption = 1;
    }
    rememberedTime = 0;
  }
  lastButtonState2 = buttonState2;

  //-------------- NFC
  if(currentTime - rememberedRfidTime >= 250) {
    rememberedRfidTime = currentTime; 

    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; 
    uint8_t uidLength;   
    
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if(success){
      String currentReadUID = ""; 
      for(int i=0; i<uidLength; i++){
        currentReadUID += String(uid[i], HEX);
      }
      UID = currentReadUID;
      
      if(currentOption == 1){
        savedUID = UID;
        savedWeight = scale.get_units(3);
        preferences.putString("uidRFID", savedUID);
        preferences.putFloat("weight", savedWeight);

        currentOption = 0;
        currentDisplay = 0;
      }
    } else {
      UID = ""; 
    }
  }
  
  if(currentTime - rememberedTime > 1000) {
    rememberedTime = currentTime;
    
    //--------------- HX711 and logic
    float weight = scale.get_units(1);
    
    if(currentOption == 0){
      if(weight < 20.0){
        isOnPad = false;
      }
      else if(weight > (savedWeight + 10.0) && UID == savedUID) {
        float currentLiquid = weight - savedWeight; 

        if (isOnPad == false) {
           isOnPad = true;
           
           if (currentLiquid < lastLiquidAmount) {
              int amountDrank = lastLiquidAmount - currentLiquid;
              
              if (amountDrank > 5) { 
                 dailyFluid += amountDrank;
                 Serial.print("Wypito: "); Serial.println(amountDrank);
              }
           }
           lastLiquidAmount = currentLiquid; 
        } 
        else {
           if (currentLiquid > lastLiquidAmount) {
              lastLiquidAmount = currentLiquid;
           }
        }
      }
    }

    //-------display
    u8g2.clearBuffer();
    if(currentDisplay == 0){
      u8g2.drawFrame(2,2,20,30);
      if(dailyFluid >= dailyFluidSet){
        u8g2.drawBox(2,2,20,30);
      }
      else{
        u8g2.drawBox(2,32-((30*dailyFluid)/dailyFluidSet),20,(30*dailyFluid)/dailyFluidSet);
      }
      
      u8g2.setCursor(25,30);
      u8g2.print("Wypito: ");
      u8g2.print(dailyFluid);
      u8g2.print(" ml");
      u8g2.setCursor(25,20);
      u8g2.print("Waga: ");
      u8g2.print(weight, 0);
      u8g2.print(" g");
    }
    else if(currentDisplay == 1){
      if(currentOption == 1){
        u8g2.setCursor(0, 15);
        u8g2.print("Trwa konfiguracja");
      }
      else {
        u8g2.setCursor(0, 15);
        u8g2.print("Konfiguracja kubka");
      }
    }
    
    u8g2.sendBuffer();
  }
}