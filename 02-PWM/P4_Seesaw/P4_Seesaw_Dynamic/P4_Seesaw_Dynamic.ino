#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN        34                       // potentiometer wiper
#define LED_COUNT      5
const int LED_PINS[LED_COUNT] = {15, 16, 17, 18, 19};   // LED0 .. LED4, each through 150 ohm to GND

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    1000
#define PWM_RES_BITS   12
#define DUTY_MAX       4095

// ---------- Brightness limits (as fractions of DUTY_MAX) ----------
#define DUTY_4_PCT     163                      // 4 %  of 4095
#define DUTY_90_PCT    3685                     // 90 % of 4095
#define DUTY_12_PCT    491                      // 12 % of 4095
#define DUTY_80_PCT    3276                     // 80 % of 4095
#define POT_MID        2047                     // middle of the pot range

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  for (int i = 0; i < LED_COUNT; i++) {
    ledcAttach(LED_PINS[i], PWM_FREQ_HZ, PWM_RES_BITS);
    ledcWrite(LED_PINS[i], 0);
  }
}

void loop() {
  int potValue = analogRead(POT_PIN);
  int duty[LED_COUNT];

  duty[0] = potValue;                                           // 0 %  -> 100 %
  duty[1] = map(potValue, 0, 4095, DUTY_4_PCT, DUTY_90_PCT);    // 4 %  -> 90 %
  if (potValue <= POT_MID) {                                    // middle LED: up, then down
    duty[2] = map(potValue, 0, POT_MID, DUTY_12_PCT, DUTY_80_PCT);
  } else {
    duty[2] = map(potValue, POT_MID, 4095, DUTY_80_PCT, DUTY_12_PCT);
  }
  duty[3] = map(potValue, 0, 4095, DUTY_90_PCT, DUTY_4_PCT);    // 90 % -> 4 %
  duty[4] = DUTY_MAX - potValue;                                // 100 % -> 0 %

  for (int i = 0; i < LED_COUNT; i++) {
    ledcWrite(LED_PINS[i], duty[i]);
  }
  delay(10);
}
