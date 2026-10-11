#include <Arduino.h>

// ---------- Hardware ----------
#define BUTTON_PIN        14     // button between GPIO14 and GND (internal pull-up used)

// ---------- Timing rules ----------
#define DEBOUNCE_MS       50     // the pin must stay at a new level longer than this
#define LONG_PRESS_MS     1000   // held longer than this = long press
#define DOUBLE_WINDOW_MS  400    // a 2nd press must start within this time, counted from the 1st press start

// ---------- Debounce variables ----------
int lastReading  = HIGH;             // raw pin reading from the previous check (may bounce)
int stableLevel  = HIGH;             // accepted button level (HIGH = released, LOW = pressed)
unsigned long lastChangeTime = 0;    // time when the raw reading last changed

// Returns true while the debounced button is held down.
bool buttonIsPressed() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {        // reading changed: restart the wait
    lastReading    = reading;
    lastChangeTime = millis();
  }
  if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
    stableLevel = reading;             // stayed at this level long enough: accept it
  }
  return (stableLevel == LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // released = HIGH, pressed = LOW
}

void loop() {
  if (buttonIsPressed()) {                         // a press has started
    unsigned long pressStart = millis();

    while (buttonIsPressed()) {                    // wait here until it is released
    }
    unsigned long pressDuration = millis() - pressStart;

    if (pressDuration > LONG_PRESS_MS) {
      Serial.println("Long press detected");
    } else {
      bool secondPress = false;
      while ((millis() - pressStart) < DOUBLE_WINDOW_MS) {   // watch for a 2nd press
        if (buttonIsPressed()) {
          secondPress = true;
          break;
        }
      }

      if (secondPress) {
        Serial.println("Double click detected");
        while (buttonIsPressed()) {                // let the 2nd press end before looking again
        }
      } else {
        Serial.println("Single click detected");
      }
    }
  }
}
