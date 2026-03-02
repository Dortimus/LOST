#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <math.h>

extern volatile float lat, longi, alt, compassDegree;
extern volatile long speed_long;
extern volatile uint8_t fix_type, hour, minute, SDState, displayConnect;
extern int batteryLevel;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET 16

Adafruit_SSD1305 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// 1. SIGNAL BARS (Restored)
void drawAnimatedBars(int x, int y, uint8_t fix) {
  int animStep = (millis() / 300) % 3;
  for (int i = 0; i < 3; i++) {
    int h = 4 + (i * 3);
    display.drawRect(x + (i * 5), y + (10 - h), 3, h, WHITE);
    // If we have a 3D fix, show all bars. Otherwise, animate searching.
    if (fix >= 3 || animStep >= i) {
      display.fillRect(x + (i * 5), y + (10 - h), 3, h, WHITE);
    }
  }
}

// 2. BATTERY ICON (Clean: Fill + Text)
void drawBatteryIcon(int x, int y, int pct) {
  display.drawRect(x, y, 26, 12, WHITE);
  display.fillRect(x + 26, y + 3, 2, 6, WHITE); 
  int fill = map(constrain(pct, 0, 100), 0, 100, 0, 22);
  if (fill > 0) display.fillRect(x + 2, y + 2, fill, 8, WHITE);
  
  display.setCursor(x + 3, y + 2);
  display.setTextColor(pct > 60 ? BLACK : WHITE);
  display.print(pct);
  display.setTextColor(WHITE);
}

// 3. COMPASS
void drawAdvancedCompass(float heading) {
  int centerX = 64, centerY = 35, radius = 20;
  display.drawCircle(centerX, centerY, radius + 5, WHITE);
  auto drawLabel = [&](const char* label, float angleOffset) {
    float rad = (angleOffset - heading - 90.0) * (M_PI / 180.0);
    int x = centerX + cos(rad) * radius;
    int y = centerY + sin(rad) * radius;
    display.setCursor(x - 3, y - 3);
    display.print(label);
  };
  drawLabel("N", 0); drawLabel("E", 90); drawLabel("S", 180); drawLabel("W", 270);
  display.fillTriangle(centerX, centerY - radius - 2, centerX - 4, centerY - radius + 5, centerX + 4, centerY - radius + 5, WHITE);
}

void display_init() {
  if (display.begin(0x3C)) {
    displayConnect = 2; 
    pinMode(OLED_CS, OUTPUT);
    digitalWrite(OLED_CS, HIGH); // Free the bus
    display.clearDisplay();
    display.display();
  }
}

int update_display(uint8_t state, uint8_t connected) {
  if (connected == 1) { 
    display_init(); 
    return 1; 
  } else if (connected == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // --- TOP BAR ---
    drawAnimatedBars(4, 2, fix_type);     // Signal Bars back in the corner
    display.setCursor(52, 2);
    display.printf("%02d:%02d", hour, minute);
    drawBatteryIcon(98, 2, batteryLevel);

    // --- BOTTOM STATUS ---
    if (SDState == 1) {
      display.fillCircle(102, 58, 2, WHITE);
      display.setCursor(108, 55); display.print("REC");
    }

    // --- MAIN CONTENT ---
    if (state == 1) {
      drawAdvancedCompass(compassDegree); 
      display.setCursor(0, 54);
      display.printf("HDG: %.1f", compassDegree);
    } else {
      display.setCursor(0, 18);
      display.printf("LAT: %.6f\nLON: %.6f\nSPD: %ld mph\nALT: %.0f FT", lat, longi, speed_long, alt);
    }
    
    display.display();
    digitalWrite(OLED_CS, HIGH); // Critical: Free SPI for SD card
  }
  return 0;
}
#endif
