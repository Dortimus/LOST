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
extern volatile int displayConnect;
double totalDistance = 0;
double lat1 = 0;
double lon1 = 0;
double lat2 = 0;
double lon2 = 0;
int lastDistanceTime = 5000;
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
      // Check if time has passed, we have a 3D fix, and coordinates aren't 0
      if ((fix_type >= 3) && (lat != 0)) {
    
        // CASE A: We don't have a starting point yet (First Fix)
        if (lat2 == 0) {
          lat2 = lat;
          lon2 = longi;
          // Optional: Serial.println("First valid coordinate locked!");
        } 
        // CASE B: We already have a starting point, so calculate movement
        else {
          double distance = haversine(lat, longi, lat2, lon2);
        
          // Update the 'previous' point to the 'current' point for the next jump
          lat2 = lat;
          lon2 = longi;
        
          // Add to your total trip distance here
          if (distance > 0.0005) { // Only add distance if it's more than ~0.5 meters
            totalDistance += distance;
          }
        } 
      }
      SD_saving(GPSfile_p);
    }
  }

  batteryLevel = checkBatteryLevel();
  update_display(displayState, displayConnect);
  SD_saving_init(GPSfile_p);
  if (SDState == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  //Serial.println("got past the LED write");

  

  //debugging
  //Serial.print("Distance: ");
  //Serial.println(totalDistance);
  //Serial.print("Heading: ");
  //Serial.println(compassDegree);
  //Serial.print("Fix type: ");
  //Serial.println(fix_type);
  //Serial.print("SDState: ");
  //Serial.println(SDState);
  //Serial.print("SDState_next: ");
  //Serial.println(SDState_next);
  //Serial.print("displayState: ");
  //Serial.println(displayState);
  //Serial.print("displayConnect: ");
  //Serial.println(displayConnect);
  //Serial.print("Battery level: ");
  //Serial.println(batteryLevel);
}
