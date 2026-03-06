#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <math.h>

extern volatile double lat, longi;
extern volatile float alt, compassDegree;
extern volatile long speed_long;
extern volatile int fix_type, hour, minute, day, month, year, SDState, displayConnect;
extern int batteryLevel;
extern double totalDistance;

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

// 3. TAPE COMPASS (Updated with NE, SE, SW, NW)
void drawTapeCompass(int x, int y, int width, float heading) {
  int center = x + width / 2;
  int degreesVisible = 45;
  float pixelsPerDegree = (float)width / degreesVisible;

  display.drawLine(x, y, x + width, y, WHITE);
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
      display.setCursor(px - (strlen(label) * 3), y - 19);
      display.print(label);
    } else if (deg % 10 == 0) {
      display.drawLine(px, y - 5, px, y, WHITE);
    }
  }

  display.setTextSize(2);
  display.setCursor(center - 18, y + 14);
  display.printf("%03d", (int)heading);
  display.drawCircle(center + 22, y + 16, 2, WHITE);
}

// 4. BIKE MODE
void drawBikeMode() {
  int center = 64;
  display.setTextSize(1);
  display.setCursor(0, 14);
  display.print("BIKE MODE");

  display.setTextSize(3);
  char spdStr[8];
  sprintf(spdStr, "%ld", speed_long);
  int spdWidth = strlen(spdStr) * 24;
  display.setCursor(center - spdWidth / 2, 24);
  display.print(spdStr);

  display.setTextSize(1);
  display.print(" mph");

  display.setCursor(0, 48);
  display.printf("DIST: %.2f mi", totalDistance);
  display.setCursor(0, 56);
  display.printf("ALT: %.0fft  HDG: %s", alt, getCardinalDirection(compassDegree).c_str());
}

// 5. CAR MODE (Lowered speed, removed line)
void drawCarMode() {
  int center = 64;
  display.setTextSize(1);
  display.setCursor(0, 14);
  display.print("CAR MODE");

  display.setTextSize(3);
  char spdStr[8];
  sprintf(spdStr, "%ld", speed_long);
  int spdWidth = strlen(spdStr) * 18;
  display.setCursor(center - spdWidth / 2 - 10, 24); 
  display.print(spdStr);
  
  display.setTextSize(2);
  display.setCursor(center + spdWidth / 2 - 8, 26);
  display.print("mph");
  
  display.setTextSize(1);
  //display.setCursor(0, 48);
  //display.printf("DIST: %.2f mi", totalDistance);
  //display.setCursor(0, 56);
  //display.printf("GPS: %.4f, %.4f", lat, longi);
  display.setCursor(0, 56);
  display.printf("ALT: %.0fft  HDG: %s", alt, getCardinalDirection(compassDegree).c_str());
}

// 6. HOME SCREEN (Simple Time/Date)
void drawHomeScreen() {
  int center = 64;
  int h12 = hour % 12 == 0 ? 12 : hour % 12;

  display.setTextSize(3);
  char timeStr[8];
  sprintf(timeStr, "%d:%02d", h12, minute);
  int timeWidth = strlen(timeStr) * 18;
  display.setCursor(center - timeWidth / 2, 18);
  display.print(timeStr);

  const char* months[] = {"JAN","FEB","MAR","APR","MAY","JUN",
                          "JUL","AUG","SEP","OCT","NOV","DEC"};
  
  display.setTextSize(1);
  char dateStr[16];
  if (month >= 1 && month <= 12) sprintf(dateStr, "%s %02d, %d", months[month-1], day, year);
  else sprintf(dateStr, "DATE ERROR");

  int dateWidth = strlen(dateStr) * 6;
  display.setCursor(center - dateWidth / 2, 45);
  display.print(dateStr);
}

void display_init() {
  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, LOW);
  delay(20);
  digitalWrite(OLED_RESET, HIGH);

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

    drawAnimatedBars(4, 2, fix_type);
    
    if (state != 0) {
      display.setCursor(52, 2);
      display.printf("%02d:%02d", hour % 12 == 0 ? 12 : hour % 12, minute);
    }
    
    drawBatteryIcon(98, 2, batteryLevel);

    if (SDState == 1) {
      display.fillCircle(115, 58, 2, WHITE);
    }

    switch(state) {
      case 0: drawHomeScreen(); break;
      case 1: drawBikeMode(); break;
      case 2: drawCarMode(); break;
      case 3: drawTapeCompass(0, 32, 128, compassDegree); break;
    }

    display.display();
    digitalWrite(OLED_CS, HIGH);
    return 2;
  }
  return 0;
}

#endif
