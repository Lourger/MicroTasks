#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN        34                       // potentiometer wiper
#define LED_COUNT      5
const int LED_PINS[LED_COUNT] = {15, 16, 17, 18, 19};   // first .. fifth LED, each through 150 ohm to GND

// ---------- PWM and ADC ranges ----------
#define PWM_FREQ_HZ    1000
#define PWM_RES_BITS   12
#define DUTY_MAX       4095                     // 100 % duty
#define ADC_MAX        4095                     // 12-bit ADC full scale
#define SEGMENT_SIZE   819                      // 4095 / 5 LEDs = about 819 ADC counts per LED

void setup() {
  analogReadResolution(12);
  for (int i = 0; i < LED_COUNT; i++) {
    ledcAttach(LED_PINS[i], PWM_FREQ_HZ, PWM_RES_BITS);
    ledcWrite(LED_PINS[i], 0);
  }
}

void loop() {
  int potValue = analogRead(POT_PIN);           // 0..4095

  // Which LED is the one that is still filling up?
  int activeLed = potValue / SEGMENT_SIZE;      // 0..5
  if (activeLed > LED_COUNT - 1) {
    activeLed = LED_COUNT - 1;                  // 4095 / 819 = 5, keep it on the last LED
  }
  int segmentStart = activeLed * SEGMENT_SIZE;
  int segmentEnd   = (activeLed == LED_COUNT - 1) ? ADC_MAX : segmentStart + SEGMENT_SIZE - 1;

  for (int i = 0; i < LED_COUNT; i++) {
    if (i < activeLed) {
      ledcWrite(LED_PINS[i], DUTY_MAX);                                          // full
    } else if (i == activeLed) {
      ledcWrite(LED_PINS[i], map(potValue, segmentStart, segmentEnd, 0, DUTY_MAX)); // partly
    } else {
      ledcWrite(LED_PINS[i], 0);                                                 // off
    }
  }
}
