#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN        34                       // potentiometer wiper
#define LED_COUNT      5
const int LED_PINS[LED_COUNT] = {15, 16, 17, 18, 19};   // LED0 .. LED4, each through 150 ohm to GND

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    1000
#define PWM_RES_BITS   12
#define DUTY_MAX       4095                     // 100 %

void setBrightness(int potValue) {
  int duty[LED_COUNT];
  duty[0] = potValue;                                   // 0 %  -> 100 %
  duty[1] = map(potValue, 0, 4095, 1024, 3071);         // 25 % -> 75 %
  duty[2] = 2048;                                       // stays at 50 % (the pivot)
  duty[3] = map(potValue, 0, 4095, 3071, 1024);         // 75 % -> 25 %
  duty[4] = DUTY_MAX - potValue;                        // 100 % -> 0 %

  for (int i = 0; i < LED_COUNT; i++) {
    ledcWrite(LED_PINS[i], duty[i]);
  }
  delay(1);
}

void setup() {
  analogReadResolution(12);
  for (int i = 0; i < LED_COUNT; i++) {
    ledcAttach(LED_PINS[i], PWM_FREQ_HZ, PWM_RES_BITS);
    ledcWrite(LED_PINS[i], 0);
  }
}

void loop() {
  int potValue = analogRead(POT_PIN);                   // 0..4095
  setBrightness(potValue);
}
