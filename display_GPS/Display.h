#ifndef BUTTONS_H
#define BUTTONS_H

#include "esp_sleep.h"

#define INTERRUPT_PIN_POWER 25
#define INTERRUPT_PIN_DISPLAY 34 
#define INTERRUPT_PIN_SD_SAVE 39 
#define INTERRUPT_PIN_CONNECTED 27

#define DEBOUNCE_TIME 250 
#define NUM_DISPLAY_STATES 1 

volatile uint8_t powerState = 0;
volatile uint8_t displayState = 0;
volatile uint8_t displayConnect = 0;
volatile uint8_t SDState = 0;      
volatile uint8_t SDState_next = 0;

volatile unsigned long last_power_time = 0;   
volatile unsigned long last_display_time = 0;
volatile unsigned long last_sd_time = 0;
volatile unsigned long last_display_connected_time = 0;

void IRAM_ATTR toggleFlagPower() {
  unsigned long now = millis();
  if (now - last_power_time > DEBOUNCE_TIME) {
    powerState = !powerState;
    last_power_time = now;
  }
}

void IRAM_ATTR updateFlagDisplay() {
  unsigned long now = millis();
  if (now - last_display_time > DEBOUNCE_TIME) {
    displayState++;
    if (displayState > NUM_DISPLAY_STATES) displayState = 0;
    last_display_time = now;
  }
}

void IRAM_ATTR toggleFlagSDSave() {
  unsigned long now = millis();
  if (now - last_sd_time > DEBOUNCE_TIME) {
    SDState_next = !SDState_next;
    last_sd_time = now;
  }
}

void IRAM_ATTR FlagDisplayChange() {
  unsigned long now = millis();
  if (now - last_display_connected_time > DEBOUNCE_TIME) {
    displayConnect = (digitalRead(INTERRUPT_PIN_CONNECTED) == LOW) ? 1 : 0;
    last_display_connected_time = now;
  }
}

void button_init() {
  pinMode(INTERRUPT_PIN_POWER, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_DISPLAY, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_SD_SAVE, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_CONNECTED, INPUT_PULLUP);
}
#endif
