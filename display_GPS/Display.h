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
  display.setTextColor(fill > 10 ? BLACK : WHITE);
  display.print(pct);
  display.setTextColor(WHITE);
}

// 3. TAPE COMPASS
void drawTapeCompass(int x, int y, int width, float heading) {
  int center = x + width / 2;
  int degreesVisible = 45;
  float pixelsPerDegree = (float)width / degreesVisible;

  // Base line
  display.drawLine(x, y, x + width, y, WHITE);

  // Triangle pointer below center
  display.fillTriangle(center, y + 1, center - 5, y + 9, center + 5, y + 9, WHITE);

  for (int d = -degreesVisible / 2; d <= degreesVisible / 2; d++) {
    int deg = ((int)(heading + d) % 360 + 360) % 360;
    int px = center + (int)(d * pixelsPerDegree);
    if (px < x || px > x + width) continue;

    const char* label = nullptr;
    if      (deg == 0)   label = "N";
    else if (deg == 45)  label = "NE";
    else if (deg == 90)  label = "E";
    else if (deg == 135) label = "SE";
    else if (deg == 180) label = "S";
    else if (deg == 225) label = "SW";
    else if (deg == 270) label = "W";
    else if (deg == 315) label = "NW";

    if (label) {
      display.drawLine(px, y - 10, px, y, WHITE);
      int labelX = px - (strlen(label) == 2 ? 4 : 2);
      display.setCursor(labelX, y - 19);
      display.print(label);
    } else if (deg % 10 == 0) {
      display.drawLine(px, y - 5, px, y, WHITE);
    } else if (deg % 5 == 0) {
      display.drawLine(px, y - 3, px, y, WHITE);
    }
  }

  // Smaller degree number (size 2)
  display.setTextSize(2);
  char degStr[8];
  sprintf(degStr, "%03d", (int)heading);
  int degWidth = strlen(degStr) * 12;
  display.setCursor(center - degWidth / 2, y + 14);
  display.print(degStr);

  // Large cardinal direction
  const char* cardinal = "";
  int h = (int)heading;
  if      (h < 23  || h >= 338) cardinal = "N";
  else if (h < 68)              cardinal = "NE";
  else if (h < 113)             cardinal = "E";
  else if (h < 158)             cardinal = "SE";
  else if (h < 203)             cardinal = "S";
  else if (h < 248)             cardinal = "SW";
  else if (h < 293)             cardinal = "W";
  else                          cardinal = "NW";

  display.setTextSize(3);
  int cardWidth = strlen(cardinal) * 18;
  display.setCursor(center - cardWidth / 2, y + 32);
  display.print(cardinal);

  display.setTextSize(1);
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
      drawTapeCompass(0, 32, 128, compassDegree);
    } else {
      display.setCursor(0, 18);
      display.printf("LAT: %.6f\nLON: %.6f\nSPD: %ld mph\nALT: %.0f FT", lat, longi, speed_long, alt);
    }

    display.display();
    digitalWrite(OLED_CS, HIGH);
    return 2;
  }
  return 0;
}

#endif
