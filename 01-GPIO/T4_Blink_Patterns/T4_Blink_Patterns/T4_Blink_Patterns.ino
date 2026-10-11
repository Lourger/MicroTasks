#include <Arduino.h>

// ---------- Hardware ----------
#define BUTTON_PIN   13                         // button between GPIO13 and GND (internal pull-up)
const int LED_PINS[4] = {15, 16, 17, 18};       // LED1..LED4, each through 150 ohm to GND

// ---------- Timing ----------
#define STEP_MS      500                        // every step of every pattern lasts 500 ms
#define DEBOUNCE_MS  50                         // the pin must stay at a new level longer than this

// ---------- Patterns ----------
// Each step is a 4-bit picture of the LEDs: bit0 = LED1 ... bit3 = LED4 (1 = ON)
const uint8_t PATTERN_0[] = {0b1111, 0b0000};                  // all ON, all OFF
const uint8_t PATTERN_1[] = {0b0101, 0b1010};                  // LED1+LED3, LED2+LED4
const uint8_t PATTERN_2[] = {0b0001, 0b0010, 0b0100, 0b1000};  // one LED runs 1 -> 4
const uint8_t PATTERN_3[] = {0b0011, 0b1100};                  // LED1+LED2, LED3+LED4
const uint8_t PATTERN_4[] = {0b1001, 0b0110};                  // LED1+LED4, LED2+LED3

#define PATTERN_COUNT  5
const uint8_t* const PATTERNS[PATTERN_COUNT] = {PATTERN_0, PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4};
const uint8_t PATTERN_LENGTH[PATTERN_COUNT]  = {2, 2, 4, 2, 2};   // steps in each pattern

// ---------- State ----------
int currentPattern = 0;              // which pattern is playing (0..4)

int lastReading = HIGH;              // debounce: raw reading from the previous check
int stableLevel = HIGH;              // debounce: accepted button level
unsigned long lastChangeTime = 0;    // debounce: when the raw reading last changed

void showStep(uint8_t step) {
  for (int i = 0; i < 4; i++) {
    int ledOn = (step >> i) & 1;     // take bit i of the step
    digitalWrite(LED_PINS[i], ledOn ? HIGH : LOW);
  }
}

bool buttonPressed() {               // true once per press (on the HIGH -> LOW change)
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastReading    = reading;
    lastChangeTime = millis();
  }
  if ((millis() - lastChangeTime) > DEBOUNCE_MS && reading != stableLevel) {
    stableLevel = reading;
    if (stableLevel == LOW) {
      return true;
    }
  }
  return false;
}

// Waits up to waitMs, checking the button the whole time.
// Returns true as soon as a press is seen (the wait ends early).
bool waitWithButtonCheck(unsigned long waitMs) {
  unsigned long start = millis();
  while (millis() - start < waitMs) {
    if (buttonPressed()) {
      return true;
    }
  }
  return false;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void loop() {
  // Play the current pattern once, step by step
  for (int s = 0; s < PATTERN_LENGTH[currentPattern]; s++) {
    showStep(PATTERNS[currentPattern][s]);

    if (waitWithButtonCheck(STEP_MS)) {                       // pressed during this step
      currentPattern = (currentPattern + 1) % PATTERN_COUNT;  // next pattern (4 wraps to 0)
      return;                                                 // loop() restarts at step 0
    }
  }
}
