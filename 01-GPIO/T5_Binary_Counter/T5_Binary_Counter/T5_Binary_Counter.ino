#include <Arduino.h>

// ---------- Hardware ----------
#define BUTTON_UP_PIN    13                     // UP button between GPIO13 and GND (internal pull-up)
#define BUTTON_DOWN_PIN  14                     // DOWN button between GPIO14 and GND (internal pull-up)
const int LED_PINS[4] = {15, 16, 17, 18};       // bit0 (LSB) .. bit3 (MSB), each through 150 ohm to GND

// ---------- Timing ----------
#define DEBOUNCE_MS  50                         // the pin must stay at a new level this long

// ---------- Counter ----------
#define COUNT_MAX  15                           // 4 bits: 0..15
uint8_t count = 0;                              // shown on the 4 LEDs

// ---------- Debounce state, one slot per button: [0] = UP, [1] = DOWN ----------
const int BUTTON_PINS[2] = {BUTTON_UP_PIN, BUTTON_DOWN_PIN};
int lastReading[2] = {HIGH, HIGH};
int stableLevel[2] = {HIGH, HIGH};
unsigned long lastChangeTime[2] = {0, 0};

bool buttonPressed(int index) {                 // true once per press of button [index]
  int reading = digitalRead(BUTTON_PINS[index]);

  if (reading != lastReading[index]) {
    lastReading[index]    = reading;
    lastChangeTime[index] = millis();
  }
  if ((millis() - lastChangeTime[index]) >= DEBOUNCE_MS && reading != stableLevel[index]) {
    stableLevel[index] = reading;
    if (stableLevel[index] == LOW) {
      return true;
    }
  }
  return false;
}

void showCount(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    int bitOn = (value >> i) & 1;               // bit i of the count
    digitalWrite(LED_PINS[i], bitOn ? HIGH : LOW);
  }
}

void setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  showCount(count);
}

void loop() {
  if (buttonPressed(0)) {                       // UP: stops at 15
    if (count < COUNT_MAX) {
      count++;
      showCount(count);
    }
  }
  if (buttonPressed(1)) {                       // DOWN: stops at 0
    if (count > 0) {
      count--;
      showCount(count);
    }
  }
}
