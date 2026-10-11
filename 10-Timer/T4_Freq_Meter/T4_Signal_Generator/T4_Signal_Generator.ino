#include <Arduino.h>

// A test signal for the frequency meter: a pot sets 100 kHz .. 8 MHz
#define SIGNAL_PIN    5             // to the meter's GPIO4
#define POT_PIN       34
#define MIN_HZ        100000.0
#define MAX_HZ        8000000.0
#define PWM_BITS      1             // 1-bit LEDC: duty 1 of 2 = 50 %, works up to 40 MHz

uint32_t setHz = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  ledcAttach(SIGNAL_PIN, (uint32_t)MIN_HZ, PWM_BITS);
  ledcWrite(SIGNAL_PIN, 1);
}

void loop() {
  // exponential pot scale: equal turns multiply the frequency by equal steps
  double position = analogRead(POT_PIN) / 4095.0;
  uint32_t wantedHz = (uint32_t)(MIN_HZ * pow(MAX_HZ / MIN_HZ, position));
  if (abs((long)wantedHz - (long)setHz) > (long)(setHz / 50)) {   // change of more than 2 %
    ledcChangeFrequency(SIGNAL_PIN, wantedHz, PWM_BITS);
    ledcWrite(SIGNAL_PIN, 1);
    setHz = wantedHz;
    Serial.printf("Wanted %lu Hz, LEDC gives %lu Hz\n",
                  (unsigned long)wantedHz, (unsigned long)ledcReadFreq(SIGNAL_PIN));
  }
  delay(200);
}
