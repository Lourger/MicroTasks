#include <Arduino.h>

// ---------- Hardware ----------
#define POT_PIN        34                       // potentiometer wiper
#define LED_COUNT      5
const int LED_PINS[LED_COUNT] = {15, 16, 17, 18, 19};   // each through 150 ohm to GND

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    1000
#define PWM_RES_BITS   12
#define DUTY_MAX       4095

// ---------- Fade speed ----------
#define MIN_DELAY_US   10                       // pot at 0 %: fastest fade
#define MAX_DELAY_US   1000                     // pot at 100 %: slowest fade
#define HOLD_FACTOR    1500                     // pause at each end = 1500 x the step delay

// Group A = GPIO15, 17, 19 (LED index 0, 2, 4); group B = GPIO16, 18 (index 1, 3)
void showBrightness(int dutyA) {
  int dutyB = DUTY_MAX - dutyA;                 // group B always does the opposite
  for (int i = 0; i < LED_COUNT; i++) {
    ledcWrite(LED_PINS[i], (i % 2 == 0) ? dutyA : dutyB);
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  for (int i = 0; i < LED_COUNT; i++) {
    ledcAttach(LED_PINS[i], PWM_FREQ_HZ, PWM_RES_BITS);
    ledcWrite(LED_PINS[i], 0);
  }
}

void loop() {
  // 1) Read the pot once per cycle and turn it into a step delay
  uint16_t fadeDelay = map(analogRead(POT_PIN), 0, 4095, MIN_DELAY_US, MAX_DELAY_US);
  Serial.println(fadeDelay);

  // 2) Group A fades in while group B fades out
  for (int duty = 0; duty <= DUTY_MAX; duty++) {
    showBrightness(duty);
    delayMicroseconds(fadeDelay);
  }
  delayMicroseconds(HOLD_FACTOR * fadeDelay);   // short pause at full brightness

  // 3) The other way round: group A fades out while group B fades in
  for (int duty = DUTY_MAX; duty >= 0; duty--) {
    showBrightness(duty);
    delayMicroseconds(fadeDelay);
  }
  delayMicroseconds(HOLD_FACTOR * fadeDelay);
}
