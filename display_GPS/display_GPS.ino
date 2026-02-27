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
  
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
  powerState = 0;
  
  Serial.println("System Ready");
}

void loop() {
  // Sleep Logic (Original)
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

  // GPS and SD Writing Logic (Original)
  if (myGNSS.checkUblox()) {
    PVTUpdate();
    if (SDState == 1 && GPSfile) {
      SD_saving(GPSfile_p);
    }
  }

  // SD Initialization and LED status (Original)
  SD_saving_init(GPSfile_p);
  if (SDState == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // REFRESH TIMER: Updates 20 times per second for smooth compass
  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 50) {
    batteryLevel = checkBatteryLevel();
    update_display(displayState, displayConnect);
    lastRefresh = millis();
  }
}
