#ifndef SD_HEADER_H
#define SD_HEADER_H

#include <SD.h>
#include <SPI.h>
#include <stdio.h>

// Tell this file that these variables exist elsewhere (GPS_header.h)
extern volatile uint16_t year;
extern volatile uint8_t month, day, hour, minute, sec, fix_type, SDState, SDState_next;
extern volatile float lat, longi, alt, compassDegree;
extern volatile long speed_long;

// Allow SD_header to talk to the display if needed
#include <Adafruit_SSD1305.h>
extern Adafruit_SSD1305 display;

// SD card chip select pin
#define SD_CS 14

void init_SD() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
}

uint8_t SD_saving_init(File* file_p) {
  if (file_p == NULL) {
    Serial.println("Null file pointer");
    return 0;
  }

  // We only run this logic when the button was just pressed (SDState_next is 1, but SDState is still 0)
  if (SDState == 0 && SDState_next == 1) {
    if (!SD.begin(SD_CS)) {
      Serial.println("Card failed, or not present");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("No SD card present!");
      display.display();
      
      SDState_next = 0;
      SDState = 0;
      delay(1000);
      return 0;
    } else {
      Serial.println("SD card initialized");
      char filename[20]; 
      
      if (fix_type >= 3) {
        // Name format: /DDHHMMSS.csv (Zero-padded so it's always 8 chars)
        sprintf(filename, "/%02d%02d%02d%02d.csv", day, hour, minute, sec);
      } else {
        // Fallback name if no GPS fix
        sprintf(filename, "/%ld.csv", millis() / 1000);
      }

      *file_p = SD.open(filename, FILE_WRITE);
      
      if (*file_p) {
        Serial.print("File opened: ");
        Serial.println(filename);
        
        if (file_p->size() == 0) {
          file_p->println("Year,Month,Day,Hour,Minute,Second,Latitude,Longitude,Altitude,Speed(mph),FixType,Heading");
          Serial.println("Header created!");
        }
        SDState = 1; // Now we are officially recording
        return 1;
      }
    }
  } else if (SDState == 1 && SDState_next == 0) {
    // Stop recording
    if (*file_p) {
      file_p->close();
      Serial.println("Closed file");
    }
    SDState = 0;
    return 1;
  }
  return 0;
}

void SD_saving(File* file_p) {
  if (!file_p || !(*file_p)) return;

  char logBuffer[128];
  // Construct the CSV line
  int written = snprintf(logBuffer, sizeof(logBuffer), 
          "%d,%d,%d,%d,%d,%d,%.6f,%.6f,%.2f,%.2f,%d,%.2f", 
          year, month, day, hour, minute, sec, 
          lat, longi, alt, (float)speed_long, fix_type, compassDegree);

  if (written < sizeof(logBuffer)) {
    file_p->println(logBuffer);
    file_p->flush(); // Keep data safe if power cuts
    Serial.println("Data Saved!");
  }
}

#endif
