#include <Arduino.h>

// ---------- Pins ----------
#define PULSE_PIN  4                // from the pulse generator (common GND!)
#define LED_PIN    2                // LED + 150 ohm resistor to GND

volatile bool ledState = false;     // changed only by the ISR

// Runs on every rising edge, however short the pulse is
void IRAM_ATTR onPulse() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(PULSE_PIN, INPUT_PULLDOWN);   // LOW while no generator is connected
  attachInterrupt(digitalPinToInterrupt(PULSE_PIN), onPulse, RISING);
}

void loop() {
  while (true) {
    // The task's fixed "important work" loop: the microcontroller is busy
    // monitoring and performing important tasks here, and this loop must not change.
  }
}
