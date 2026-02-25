#include <SD.h>
#include <SPI.h>
#include <stdio.h>

//SD card chip select pin
#define SD_CS 14

void init_SD () {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
}

uint8_t SD_saving_init (File* file_p) {
  //Takes current SD state and next SD state and creates a file to save the current trip
  if (file_p == NULL) {
    Serial.println("Null file pointer");
    return 0;
  }

  if (SDState == 0 && SDState_next == 1) {
    //wasn't writing and is now supposed to begin writing
    //see if the card is present and can be initialized check using CD pin
    if (!SD.begin(SD_CS)) {
      Serial.println("Card failed, or not present"); //display on the display?
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("No SD card present!");
      display.display();
      //Return a failed value, continue loop.
      SDState_next = 0;
      SDState = 0;
      delay(1000);
      return 0;
    } else {
      Serial.println("SD card initialized");
      //Card is recognized, create a file with a name
      uint16_t num_files = 0;
      char filename[14] = "/XXXXXXXX.csv";
      if (fix_type == 3) {
        //If we have MMDDYYMMSS data, use that to name the file
        sprintf(filename, "/%d%d%d%d.csv", day % 100, hour % 100, minute % 100, sec % 100);
      } else {
        //If no good fix, use the millis to name file - THIS COULD BE BETTER!
        sprintf(filename, "/%d.csv", millis() % 100000000);
      }
      *file_p = SD.open(filename, FILE_WRITE);  //name must be like "/XXXXXXXX.csv"
      Serial.print("File opened named ");
      Serial.println(filename);
      if (file_p->size() == 0) {
        //If the file is empty, add a header at the top
        file_p->print("Year");
        file_p->print(",");
        file_p->print("Month");
        file_p->print(",");
        file_p->print("Day");
        file_p->print(",");
        file_p->print("Hour");
        file_p->print(",");
        file_p->print("Minute");
        file_p->print(",");
        file_p->print("Second");
        file_p->print(",");
        file_p->print("Latitude");
        file_p->print(",");
        file_p->print("Longitude");
        file_p->print(",");
        file_p->print("Altitude");
        file_p->print(",");
        file_p->print("Speed (mph)");
        file_p->print(",");
        file_p->print("Fix type");
        file_p->print(",");
        file_p->println("Heading (degrees from North)");
        SDState = 1;
        Serial.println("File + header created!");
        return 1;
      }
    }
  } else if (SDState == 1 && SDState_next == 0) {
    //was writing and is now supposed to stop
    file_p->close();
    Serial.println("Closed file");
    SDState = 0;
    return 1;
  }
  Serial.println("Function called, but nothing happened.");
  return 0;
}

void SD_saving(File* file_p) {
  Serial.println("About to write to csv");
  //if (!file_p || !(*file_p)) return;
  // 128 bytes is plenty for one CSV line
  char logBuffer[128]; 

  // Construct the string safely
  int written = snprintf(logBuffer, sizeof(logBuffer), 
         "%d,%d,%d,%d,%d,%d,%.6f,%.6f,%.2f,%.2f,%d,%.6f", 
         year, month, day, hour, minute, sec, 
         lat, longi, alt, speed_long, fix_type, compassDegree);

  // Check if we exceeded the buffer size
  if (written >= sizeof(logBuffer)) {
    Serial.println("Buffer overflow detected! Increase logBuffer size.");
  } else {
    file_p->println(logBuffer);
  }
  
  file_p->flush(); // Ensures data is saved even if power is lost
  Serial.println("Data Saved!");
}
