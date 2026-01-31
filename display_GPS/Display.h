#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Display Definitions ---
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET); // Adjust dimensions if needed (e.g., 128, 32)
#define SCREEN_ADDRESS 0x3C // I2C address for most SSD1306 displays (use I2C scanner if unsure)


int update_display (uint8_t displayState) {
  switch (displayState) {
    case 1:
      //do something - compass
      break;
    case 2:
      //do something else - speed + alt + distance + elevation gain?
      break;
    case 3:
      //do something else - 
      break;
    case 4:
      //do something else
      break;
    default:
      //default behavior, display all
      display.clearDisplay();
      display.setCursor(0, 0);
      //Time
      display.print("Time: ");
      display.print(hour);
      display.print(F(":"));
      display.print(minute);
      display.print(F(":"));
      display.println(sec);
      //Fix type
      display.print(F("Fix type: "));
      display.println(fix_type);
      //Latitude
      display.print(F("Lat: "));
      display.println(lat);
      //Longitude
      display.print(F("Long: "));
      display.print(longi);
      display.println(F(" (deg.)"));
      //Altitude
      display.print(F("Alt: "));
      display.print(alt);
      display.println(F(" (m)"));
      //Speed
      display.print(F("Speed: "));
      display.print(speed_long);
      display.print(F(" (m/s)"));
      //Update display
      display.display();
      break;
  }
  return 1;
}

void display_init() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  //display.println("Initializing GNSS...");
  //display.display();
  Serial.println("Display initialized");
}