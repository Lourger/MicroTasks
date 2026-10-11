#include <Arduino.h>
#include <math.h>

// ---------- Pins ----------
#define DAC1_PIN     25             // sine wave 1
#define DAC2_PIN     26             // sine wave 2, shifted
#define POT_PIN      34             // phase shift 0..360 degrees

// ---------- Wave ----------
#define TABLE_SIZE   256            // samples per period
#define SAMPLE_US    100            // pause after each sample

uint8_t sineTable[TABLE_SIZE];

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  for (int i = 0; i < TABLE_SIZE; i++) {   // one period, 0..255 around 127.5
    sineTable[i] = (uint8_t)(127.5 + 127.5 * sin(2.0 * PI * i / TABLE_SIZE));
  }
}

void loop() {
  // 1) Once per period: pot -> degrees -> offset in table steps
  int potValue = analogRead(POT_PIN);          // 0..4095
  float degreesShift = potValue / 11.375f;     // 4095 / 360 = 11.375
  int offset = (int)(degreesShift / 360.0f * TABLE_SIZE);

  // 2) One full period on both DACs
  for (int i = 0; i < TABLE_SIZE; i++) {
    dacWrite(DAC1_PIN, sineTable[i]);
    dacWrite(DAC2_PIN, sineTable[(i + offset) % TABLE_SIZE]);   // wraps past the end
    delayMicroseconds(SAMPLE_US);
  }
}
