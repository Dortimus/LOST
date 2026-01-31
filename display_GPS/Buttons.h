#define NUM_DISPLAY_STATES 4

volatile uint8_t powerState = 0;
volatile uint8_t displayState = 0;
volatile uint8_t SDState = 0;
volatile uint8_t SDState_next = 0;
volatile uint8_t SD_debounce = 0;
volatile uint8_t Display_debounce = 0;
volatile uint8_t Power_debounce = 0;

volatile unsigned long last_power_time = 0;
volatile unsigned long last_display_time = 0;
volatile unsigned long last_sd_time = 0;

#define INTERRUPT_PIN_POWER 14
#define INTERRUPT_PIN_DISPLAY 32
#define INTERRUPT_PIN_SD_SAVE 33

#define DEBOUNCE_TIME 20 //in ms

void button_init () {
  pinMode(INTERRUPT_PIN_POWER, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_DISPLAY, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_SD_SAVE, INPUT_PULLUP);
}

void IRAM_ATTR toggleFlagPower () {
  unsigned long now = millis();
  if (now - last_power_time > DEBOUNCE_TIME) {
    powerState = !powerState;
    last_power_time = now;
  }
}

void IRAM_ATTR updateFlagDisplay () {
  unsigned long now = millis();
  if (now - last_display_time > DEBOUNCE_TIME) {
    displayState++;
    if (displayState > NUM_DISPLAY_STATES) {
      displayState = 0;
    }
    last_display_time = now;
  }
}

void IRAM_ATTR toggleFlagSDSave () {
  unsigned long now = millis();
  if (now - last_sd_time > DEBOUNCE_TIME) {
    SDState_next = !SDState; 
    last_sd_time = now;
  }
}