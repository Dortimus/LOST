#define NUM_DISPLAY_STATES 4

volatile uint8_t powerState = 0;
volatile uint8_t displayState = 0;
volatile uint8_t SDState = 0;

#define INTERRUPT_PIN_POWER 14
#define INTERRUPT_PIN_DISPLAY 32
#define INTERRUPT_PIN_SD_SAVE 33

void button_init () {
  pinMode(INTERRUPT_PIN_POWER, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_DISPLAY, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_SD_SAVE, INPUT_PULLUP);
}

//Need to include debounce for the buttons.

void IRAM_ATTR toggleFlagPower () {
  Serial.println("Hey you pushed the button!");
  if (powerState == 0) {
    powerState = 1;
  } else {
    powerState = 0;
  }
}

void IRAM_ATTR updateFlagDisplay () {
  if (displayState < NUM_DISPLAY_STATES) {
    displayState += 1;
  } else {
    displayState = 0;
  }
}

void IRAM_ATTR toggleFlagSDSave () {
  if (SDState == 0) {
    SDState = 1;
  } else {
    SDState = 0;
  }
}