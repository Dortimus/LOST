#include <Wire.h>
#include <SD.h>
#include <FS.h>
#include "GPS_header.h"
#include "Buttons.h"
#include "Display.h"
#include "mag.h"


void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C communication

  // Initialize SSD1306 display
  //if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  //  Serial.println(F("SSD1306 allocation failed"));
  //  for(;;); // Don't proceed, loop forever
  //}

  //Initialize magnetometer
  init_mag();

  //Initial setup of display.
  //display_init();

  // Initialize u-blox GNSS
  if (myGNSS.begin(Wire)) { // Use Wire for I2C
    Serial.println(F("u-blox GNSS initialized successfully!"));
    //display.setCursor(0, 9);
    //display.println("GNSS Ready");
    //display.display();
    myGNSS.setI2COutput(COM_TYPE_UBX); // Set I2C port to output UBX messages (less noisy than NMEA)
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save settings
  } else {
    Serial.println(F("Failed to initialize u-blox GNSS"));
    //display.setCursor(0, 19);
    //display.println("GNSS Error!");
    //display.display();
    while(1);
  }

  //initilaize buttons
  button_init();

  //Interrupts for the user inputs
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_POWER), toggleFlagPower, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_DISPLAY), updateFlagDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_SD_SAVE), toggleFlagSDSave, FALLING);
}

float compassDegree;

void loop() {
  //Update PVT variables fix_type, lat, longi, alt, speed.
  //Perhaps if SDState=1 then it will save to the SD card?
  PVTUpdate(SDState);

  //Power
  if (powerState == 0) {
    //do something
  } else {
    //do something else
  }
  
  //Display
  //update_display(displayState);

  //Update compass heading, might put this inside the PVTUpdate, it's kind of PVT related
  compassDegree = bmm350.getCompassDegree();
  //Printing
  Serial.print("the angle between the pointing direction and north (counterclockwise) is:");
  Serial.println(compassDegree);
  Serial.println("--------------------------------");
}
