#ifndef BUTTONS_H
#define BUTTONS_H

#include "esp_sleep.h"

// --- PIN DEFINITIONS ---
#define INTERRUPT_PIN_POWER   25
#define INTERRUPT_PIN_DISPLAY 34 
#define INTERRUPT_PIN_SD_SAVE 39 
#define INTERRUPT_PIN_CONNECTED 27

#define DEBOUNCE_TIME 250 
#define NUM_DISPLAY_STATES 1 
#define NUM_DISPLAY_STATES 2

// --- STATE VARIABLES ---
volatile int powerState = 1;
volatile int displayState = 0;
volatile int displayConnect = 0;
volatile int SDState = 0;      
volatile int SDState_next = 0;

// --- DEBOUNCE TIMERS ---
volatile unsigned long last_power_time = 0;   
volatile unsigned long last_display_time = 0;
volatile unsigned long last_sd_time = 0;
volatile unsigned long last_display_connected_time = 0;
volatile unsigned long last_display_disconnected_time = 0;

// --- INTERRUPT FUNCTIONS ---
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
  delay(500); // Your original delay for connection stability
  unsigned long now = millis();
  if (now - last_display_connected_time > DEBOUNCE_TIME) {
    if (digitalRead(INTERRUPT_PIN_CONNECTED) == LOW) {
      displayConnect = 1;
      last_display_connected_time = now;
    } else if (digitalRead(INTERRUPT_PIN_CONNECTED) == HIGH) {
      displayConnect = 0;
      last_display_disconnected_time = now;
    }
  }
}

void button_init() {
  pinMode(INTERRUPT_PIN_POWER, INPUT);
  pinMode(INTERRUPT_PIN_DISPLAY, INPUT);
  pinMode(INTERRUPT_PIN_SD_SAVE, INPUT);
  pinMode(INTERRUPT_PIN_CONNECTED, INPUT_PULLUP);
}

#endif
