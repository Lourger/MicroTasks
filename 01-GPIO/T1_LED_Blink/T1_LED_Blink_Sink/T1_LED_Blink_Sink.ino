#include <Arduino.h>

// ---------- Hardware (sink wiring) ----------
#define LED_PIN     4      // 3.3 V -> 150 ohm resistor -> LED anode, LED cathode -> GPIO4

// ---------- Timing ----------
#define LED_ON_MS   400    // how long the LED stays ON
#define LED_OFF_MS  800    // how long the LED stays OFF

void setup() {
  pinMode(LED_PIN, OUTPUT);      // make GPIO4 an output pin
  digitalWrite(LED_PIN, HIGH);   // start with the LED OFF (both LED ends at 3.3 V)
}

void loop() {
  digitalWrite(LED_PIN, LOW);    // pin goes to 0 V: current flows into the pin, LED ON
  delay(LED_ON_MS);              // wait 400 ms

  digitalWrite(LED_PIN, HIGH);   // pin goes to 3.3 V: no voltage across the LED, LED OFF
  delay(LED_OFF_MS);             // wait 800 ms
}
