#include <Wire.h>
#include <SD.h>
#include <FS.h>
#include "GPS_header.h"
#include "Buttons.h"
#include "Display.h"
#include "SD_header.h"

#define LED_PIN 4

File GPSfile;
File* GPSfile_p = &GPSfile;
extern volatile uint8_t displayConnect;
int batteryLevel = 0;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  
  pinMode(LED_PIN, OUTPUT);
  display_init();
  init_mag();
  init_gps();
  button_init();
  init_SD();

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_POWER), toggleFlagPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), updateFlagDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_SD_SAVE), toggleFlagSDSave, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_CONNECTED), FlagDisplayChange, CHANGE);
  //Pin 25, enable low (pullup resistor)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
  powerState = 0;
  
  Serial.println("System Ready");
}

void loop() {
  if (powerState == 1) {
    Serial.println("Sleeping...");
    delay(2000);
    display.clearDisplay();
    display.display();
    if (SDState == 1) {
      GPSfile.flush();
      GPSfile.close();
    }
    esp_deep_sleep_start();
  }

  if (myGNSS.checkUblox()) {
    PVTUpdate();
    if (fix_type >= 3) {
      Serial.printf("CONNECTED (Sats: %d)\n", (int)myGNSS.getSIV());
    } else {
      Serial.println("SEARCHING...");
    }
    if (SDState == 1 && GPSfile) {
      SD_saving(GPSfile_p);
    }
  }

  batteryLevel = checkBatteryLevel();
  update_display(displayState, displayConnect);
  SD_saving_init(GPSfile_p);
  digitalWrite(LED_PIN, SDState);

  

  //debugging
  Serial.print("SDState: ");
  Serial.println(SDState);
  Serial.print("SDState_next: ");
  Serial.println(SDState_next);
  Serial.print("displayState: ");
  Serial.println(displayState);
  Serial.print("displayConnect: ");
  Serial.println(displayConnect);
  Serial.print("Battery level: ");
  Serial.println(batteryLevel);
}