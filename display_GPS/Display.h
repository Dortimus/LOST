#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <math.h>

extern volatile uint8_t hour, minute, SDState, displayConnect, fix_type;
extern int batteryLevel;
extern volatile float lat, longi, compassDegree;
extern volatile long speed_long;
extern float getCompassDegree();

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC 21
#define OLED_CS 17
#define OLED_RESET 16

Adafruit_SSD1305 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

void drawClock(int x, int y) {
  display.setCursor(x, y);
  display.printf("%02d:%02d", hour, minute);
}

void drawBatteryIcon(int x, int y, int percentage) {
  display.drawRect(x, y, 26, 12, WHITE);
  display.fillRect(x + 26, y + 3, 2, 6, WHITE);
  int fillWidth = map(constrain(percentage, 0, 100), 0, 100, 0, 22);
  if (fillWidth > 0) display.fillRect(x + 2, y + 2, fillWidth, 8, WHITE);
  display.setCursor(x + 4, y + 2);
  display.setTextColor(percentage > 50 ? BLACK : WHITE);
  display.print(percentage);
  display.setTextColor(WHITE);
}

void drawRecordingStatus(int x, int y) {
  if (SDState == 1) {
    display.fillCircle(x + 2, y + 3, 2, WHITE);
    display.setCursor(x + 8, y);
    display.print("REC");
  }
}

void drawAdvancedCompass(float heading) {
  int centerX = 64, centerY = 38, radius = 18;
  display.drawCircle(centerX, centerY, radius + 4, WHITE);
  auto drawLabel = [&](const char* label, float angleOffset) {
    float rad = (angleOffset - heading - 90.0) * (M_PI / 180.0);
    int x = centerX + cos(rad) * radius;
    int y = centerY + sin(rad) * radius;
    display.setCursor(x - 3, y - 3);
    display.print(label);
  };
  drawLabel("N", 0); drawLabel("E", 90); drawLabel("S", 180); drawLabel("W", 270);
  display.fillTriangle(centerX, centerY-radius-2, centerX-4, centerY-radius+5, centerX+4, centerY-radius+5, WHITE);
}

void display_init() {
  if(display.begin(0x3C)) {
    displayConnect = 2;
    pinMode(OLED_CS, OUTPUT); digitalWrite(OLED_CS, HIGH);
    display.clearDisplay(); display.display();
  }
}

int update_display(uint8_t state, uint8_t connected) {
  if (connected == 1) { display_init(); return 1; }
  if (connected == 2) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    
    // Header
    display.setCursor(2, 2); display.print(fix_type >= 3 ? "FIX" : "SCH");
    drawClock(52, 2);
    drawBatteryIcon(98, 2, batteryLevel);
    drawRecordingStatus(98, 54);

    if (state == 1) {
      compassDegree = getCompassDegree(); // Polled every frame
      drawAdvancedCompass(compassDegree);
      display.setCursor(0, 54); display.printf("HDG: %.1f", compassDegree);
    } else {
      display.setCursor(0, 18);
      display.printf("LAT: %.6f\nLON: %.6f\nSPD: %ld mph", lat, longi, speed_long);
    }
    display.display();
    return 1;
  }
  return 0;
}
#endif
