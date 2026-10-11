#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN         34          // potentiometer wiper (ends to 3.3 V and GND)

// ---------- Timing ----------
#define SAMPLE_EVERY_MS 100

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  int potValue = analogRead(POT_PIN);
  Serial.print(">pot:");            // Teleplot format: >name:value
  Serial.println(potValue);
  delay(SAMPLE_EVERY_MS);
}
