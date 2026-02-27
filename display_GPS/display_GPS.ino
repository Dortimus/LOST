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
unsigned long lastRefresh = 0;

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
  
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
}

void loop() {
  if (powerState == 1) {
    display.clearDisplay(); display.display();
    if (SDState == 1) { GPSfile.flush(); GPSfile.close(); }
    esp_deep_sleep_start();
  }

  // GPS Update (background)
  if (myGNSS.checkUblox()) {
    PVTUpdate();
    if (SDState == 1 && GPSfile) SD_saving(GPSfile_p);
  }

  // Instant Button Sync
  if (SDState_next != SDState) {
    SDState = SDState_next;
    if (SDState == 1) { SD_saving_init(GPSfile_p); digitalWrite(LED_PIN, HIGH); }
    else { if (GPSfile) { GPSfile.flush(); GPSfile.close(); } digitalWrite(LED_PIN, LOW); }
  }

  // Refresh Screen at ~20 FPS (50ms)
  if (millis() - lastRefresh > 50) {
    batteryLevel = checkBatteryLevel();
    update_display(displayState, displayConnect);
    lastRefresh = millis();
  }
}
