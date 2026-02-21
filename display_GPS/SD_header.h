#include <SD.h>
#include <SPI.h>
#include <stdio.h>

//SD card chip select pin
#define SD_CS 4

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
        //If we have MMDDYYHHMM data, use that to name the file
        sprintf(filename, "/%d%d%d%d%d.csv", month % 100, day % 100, year % 100, hour % 100, minute % 100);
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
  if (!file_p || !(*file_p)) return;
  Serial.println("About to write to csv");
  file_p->print(year); file_p->print(",");
  file_p->print(month); file_p->print(",");
  file_p->print(day); file_p->print(",");
  file_p->print(hour); file_p->print(",");
  file_p->print(minute); file_p->print(",");
  file_p->print(sec); file_p->print(",");
  file_p->print(lat, 7); file_p->print(","); // 7 decimal places for GPS precision
  file_p->print(longi, 7); file_p->print(",");
  file_p->print(alt, 2); file_p->print(",");
  file_p->print(speed_long); file_p->print(",");
  file_p->print(fix_type); file_p->print(",");
  file_p->println(compassDegree); 
  
  file_p->flush(); // Ensures data is saved even if power is lost
  Serial.println("Data Saved!");
}
