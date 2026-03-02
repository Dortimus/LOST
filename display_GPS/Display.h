#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <math.h>

extern volatile float lat, longi, alt, compassDegree;
extern volatile long speed_long;
extern volatile int fix_type, hour, minute, SDState, displayConnect;
extern int batteryLevel;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET 16

Adafruit_SSD1305 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// 1. TOP BAR: SIGNAL BARS
void drawAnimatedBars(int x, int y, int fix) {
  int animStep = (millis() / 300) % 3;
  for (int i = 0; i < 3; i++) {
    int h = 4 + (i * 3);
    display.drawRect(x + (i * 5), y + (10 - h), 3, h, WHITE);
    if (fix >= 3 || animStep >= i) {
      display.fillRect(x + (i * 5), y + (10 - h), 3, h, WHITE);
    }
  }
}

// 2. TOP BAR: BATTERY
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

// 3. RADAR COMPASS (Replaced the "Advanced" one)
void drawRadarCompass(int centerX, int centerY, int radius, float heading) {
  // Draw Stationary Radar Grid
  display.drawCircle(centerX, centerY, radius, WHITE);       // Outer ring
  display.drawCircle(centerX, centerY, radius/2, WHITE);     // Inner ring
  display.drawLine(centerX - radius, centerY, centerX + radius, centerY, WHITE); // Horiz line
  display.drawLine(centerX, centerY - radius, centerX, centerY + radius, WHITE); // Vert line

  // Draw Rotating "Sweep" Needle
  float angleRad = (heading - 90.0) * (M_PI / 180.0);
  int xEnd = centerX + radius * cos(angleRad);
  int yEnd = centerY + radius * sin(angleRad);
  
  display.drawLine(centerX, centerY, xEnd, yEnd, WHITE);
  display.fillCircle(xEnd, yEnd, 3, WHITE); // Radar "Target" blip at the end

  // Stationary Cardinal Labels
  display.setCursor(centerX - 3, centerY - radius - 10); display.print("N");
  display.setCursor(centerX + radius + 5, centerY - 3);  display.print("E");
  display.setCursor(centerX - 3, centerY + radius + 3);  display.print("S");
  display.setCursor(centerX - radius - 10, centerY - 3); display.print("W");
}

void display_init() {
  if (display.begin(0x3C)) {
    displayConnect = 2; 
    pinMode(OLED_CS, OUTPUT);
    digitalWrite(OLED_CS, HIGH); 
    display.clearDisplay();
    display.display();
  }
}

int update_display(int state, int connected) {
  if (connected == 1) { 
    display_init(); 
    return 1; 
  } else if (connected == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // --- TOP BAR ---
    drawAnimatedBars(4, 2, fix_type);
    display.setCursor(52, 2);
    display.printf("%02d:%02d", hour, minute);
    drawBatteryIcon(98, 2, batteryLevel);

    // --- BOTTOM STATUS ---
    if (SDState == 1) {
      display.fillCircle(105, 58, 2, WHITE);
      display.setCursor(110, 55); display.print("REC");
    }

    // --- MAIN CONTENT ---
    if (state == 1) {
      // Draw the Radar at Center X:64, Y:36, Radius:18
      drawRadarCompass(64, 36, 18, compassDegree);
      display.setCursor(0, 56);
      display.printf("HDG: %03d", (int)compassDegree);
    } else {
      display.setCursor(0, 18);
      display.printf("LAT: %.6f\nLON: %.6f\nSPD: %ld mph\nALT: %.0f FT", lat, longi, speed_long, alt);
    }
    
    display.display();
    digitalWrite(OLED_CS, HIGH); // Free SPI for SD card
    return 2;
  }
  return 0;
}
#endif
