#include <Wire.h>
#include <SD.h>
#include <FS.h>
#include "GPS_header.h"
#include "Buttons.h"
#include "Display.h"
#include "mag.h"
#include "SD_header.h"

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C communication
  Wire.setClock(400000); //Make it snappy!
  
  // Initialize SSD1306 display
  display_init();

  //Initialize magnetometer
  init_mag();

  //Initialize GPS
  init_gps();

  //initilaize buttons
  button_init();

  //Initialize SD pins
  init_SD();

  //Interrupts for the user inputs
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_POWER), toggleFlagPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), updateFlagDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_SD_SAVE), toggleFlagSDSave, FALLING);
  Serial.println("Finished setup");
}

float compassDegree;
File GPSfile;
File* GPSfile_p = &GPSfile;

unsigned long loopTime = 0;
unsigned long loopTimeOld = millis();


void loop() {
  loopTime = millis();
  Serial.print("Loop time: ");
  Serial.println(loopTime - loopTimeOld);
  loopTimeOld = loopTime;

  //Update PVT variables fix_type, lat, longi, alt, speed.
  if (myGNSS.checkUblox()) {
    PVTUpdate();
    update_display(displayState);
    if (SDState == 1 && GPSfile) {
      SD_saving(GPSfile_p);
      Serial.println("Wrote data to the SD card");
    }
  }

  //Power
  if (powerState == 0) {
    //do something
  } else {
    //do something else
  }

  //Testing
  Serial.print("SDstate: ");
  Serial.println(SDState);
  Serial.print("SDstate_next: ");
  Serial.println(SDState_next);

  //SD card saving edge detection + file creation stuff
  SD_saving_init(GPSfile_p);

  //Update compass heading, might put this inside the PVTUpdate, it's kind of PVT related
  //compassDegree = bmm350.getCompassDegree();
  //Printing
  //Serial.print("the angle between the pointing direction and north (counterclockwise) is:");
  //Serial.println(compassDegree);
  Serial.println("--------------------------------");



}
