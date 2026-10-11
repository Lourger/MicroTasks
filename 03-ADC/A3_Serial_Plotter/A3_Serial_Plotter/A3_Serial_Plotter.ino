#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN         34          // potentiometer wiper (ends to 3.3 V and GND)

// ---------- Timing ----------
#define SAMPLE_EVERY_MS 100         // 10 points per second on the plot

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);         // 0..4095
}

void loop() {
  int potValue = analogRead(POT_PIN);
  Serial.println(potValue);         // one number per line = one point on the plot
  delay(SAMPLE_EVERY_MS);
}
