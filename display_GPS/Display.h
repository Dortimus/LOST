#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <math.h>

extern volatile uint8_t SDState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC    21
#define OLED_CS    17
#define OLED_RESET 16

Adafruit_SSD1305 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);


void drawAdvancedCompass(float heading) {
  int centerX = 64; 
  int centerY = 35;
  int radius = 20;

  display.drawCircle(centerX, centerY, radius + 5, WHITE);

  // Labels that rotate so North is always "Physical North"
  auto drawLabel = [&](const char* label, float angleOffset) {
    float rad = (angleOffset - heading - 90.0) * (M_PI / 180.0);
    int x = centerX + cos(rad) * (radius);
    int y = centerY + sin(rad) * (radius);
    display.setCursor(x - 3, y - 3);
    display.print(label);
  };

  drawLabel("N", 0); drawLabel("E", 90); drawLabel("S", 180); drawLabel("W", 270);

  // Static needle pointing to top of screen
  display.fillTriangle(centerX, centerY - radius - 2, centerX - 4, centerY - radius + 5, centerX + 4, centerY - radius + 5, WHITE);
  display.drawLine(centerX, centerY - radius, centerX, centerY + 10, WHITE);
}

int update_display(uint8_t state) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (state == 1) {
    display.setCursor(40, 0);
    display.print(F("COMPASS"));
    drawAdvancedCompass(compassDegree); 
    display.setCursor(0, 56);
    display.printf("HDG: %.1f", compassDegree);
  } else {
    display.setCursor(0, 0);
    display.print(fix_type >= 3 ? F("SAT: CONNECTED") : F("SAT: SEARCHING..."));
    display.setCursor(0, 15);
    display.printf("LAT: %.6f\n", lat);
    display.printf("LON: %.6f\n", longi);
    display.printf("SPD: %ld km/s\n", speed_long);
    display.setCursor(0, 56);
    display.printf("FIX:%d | LOG:%s", fix_type, SDState ? "ON" : "OFF");
  }
  display.display();
  return 1;
}

void display_init() {
  if(!display.begin(0x3C)) {
    Serial.println(F("SSD1305 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(OLED_CS, OUTPUT);
  digitalWrite(OLED_CS, HIGH);
  display.begin();
  display.clearDisplay();
  display.display();
}
#endif