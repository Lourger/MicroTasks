#include <Arduino.h>

// Same two waves with the Arduino LEDC functions (core 3.x)
#define WAVE1_PIN      4
#define WAVE2_PIN      5
#define WAVE1_FREQ_HZ  307000
#define WAVE2_FREQ_HZ  570000
#define PWM_BITS       2            // duty 0..3
#define DUTY_50        2            // 2 of 4 = 50 %

void setup() {
  Serial.begin(115200);
  ledcAttach(WAVE1_PIN, WAVE1_FREQ_HZ, PWM_BITS);
  ledcAttach(WAVE2_PIN, WAVE2_FREQ_HZ, PWM_BITS);
  ledcWrite(WAVE1_PIN, DUTY_50);
  ledcWrite(WAVE2_PIN, DUTY_50);
  Serial.printf("Wave 1: %lu Hz, wave 2: %lu Hz\n",
                (unsigned long)ledcReadFreq(WAVE1_PIN), (unsigned long)ledcReadFreq(WAVE2_PIN));
}

void loop() {
}
