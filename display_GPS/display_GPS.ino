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


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  
  pinMode(LED_PIN, OUTPUT);
  //display_init();
  //init_mag();
  init_gps();
  button_init();
  init_SD();

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_POWER), toggleFlagPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), updateFlagDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_SD_SAVE), toggleFlagSDSave, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), FlagDisplayConnected, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), FlagDisplayDisconnected, FALLING);
  
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

  update_display(displayState, displayConnect);
  SD_saving_init(GPSfile_p);
  digitalWrite(LED_PIN, SDState);
  Serial.println(SDState);
}