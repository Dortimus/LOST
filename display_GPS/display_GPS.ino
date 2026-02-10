#include <Wire.h>
#include <SD.h>
#include <FS.h>
#include "GPS_header.h"
#include "Buttons.h"
#include "Display.h"
#include "SD_header.h"

File GPSfile;
File* GPSfile_p = &GPSfile;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  
  display_init();
  init_mag();
  init_gps();
  button_init();
  init_SD();

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_POWER), toggleFlagPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), updateFlagDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_SD_SAVE), toggleFlagSDSave, FALLING);
  
  Serial.println("System Ready");
}

void loop() {

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

  compassDegree = bmm350.getCompassDegree();
  update_display(displayState);
  SD_saving_init(GPSfile_p);
}