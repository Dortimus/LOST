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

// --- STATE VARIABLES ---
volatile uint8_t powerState = 1;
volatile uint8_t displayState = 0;
volatile uint8_t displayConnect = 0;
volatile uint8_t SDState = 0;      // Added to fix the previous 'SDState' error
volatile uint8_t SDState_next = 0;

// --- DEBOUNCE TIMERS ---
volatile unsigned long last_power_time = 0;   // Added to fix your current error
volatile unsigned long last_display_time = 0;
volatile unsigned long last_sd_time = 0;
volatile unsigned long last_display_connected_time = 0;
volatile unsigned long last_display_disconnected_time = 0;

// --- INTERRUPT FUNCTIONS ---
void IRAM_ATTR toggleFlagPower() {
  //Serial.println("Power button pushed");
  unsigned long now = millis();
  if (now - last_power_time > DEBOUNCE_TIME) {
    powerState = !powerState;
    last_power_time = now;
  }
}

void IRAM_ATTR updateFlagDisplay() {
  //Serial.println("Display button pushed");
  unsigned long now = millis();
  if (now - last_display_time > DEBOUNCE_TIME) {
    displayState++;
    if (displayState > NUM_DISPLAY_STATES) displayState = 0;
    last_display_time = now;
  }
}

void IRAM_ATTR toggleFlagSDSave() {
  //Serial.println("SD button pushed");
  unsigned long now = millis();
  if (now - last_sd_time > DEBOUNCE_TIME) {
    SDState_next = !SDState_next;
    last_sd_time = now;
  }
}

void IRAM_ATTR FlagDisplayChange() {
  delay(500);
  unsigned long now = millis();
  if (now - last_display_connected_time > DEBOUNCE_TIME) {
    if (digitalRead(INTERRUPT_PIN_CONNECTED) == LOW) {
      //Serial.println("Display connected");
      displayConnect = 1;
      last_display_connected_time = now;
    } else if (digitalRead(INTERRUPT_PIN_CONNECTED) == HIGH) {
      //Serial.println("Display disconnected");
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