#include <Arduino.h>

// ---------- Hardware ----------
#define LED_PIN        14       // GPIO14 -> 150 ohm -> LED -> GND

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    1000     // 1 kHz: fast enough that the eye sees no flicker
#define PWM_RES_BITS   12       // 12-bit duty: 0..4095
#define DUTY_MAX       4095     // (1 << 12) - 1 = 100 % duty

// ---------- Ramp timing ----------
#define RISE_STEP_US   488      // 2000 ms / 4096 steps = about 488 us per step
#define FALL_STEP_US   244      // 1000 ms / 4096 steps = about 244 us per step

void setup() {
  Serial.begin(115200);
  if (!ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_RES_BITS)) {   // give LED_PIN a PWM channel
    Serial.println("PWM setup failed on LED_PIN");
  }
  ledcWrite(LED_PIN, 0);                                    // start dark (0 % duty)
}

void loop() {
  for (int duty = 0; duty <= DUTY_MAX; duty++) {            // 0 % -> 100 % in about 2 s
    ledcWrite(LED_PIN, duty);
    delayMicroseconds(RISE_STEP_US);
  }

  for (int duty = DUTY_MAX; duty >= 0; duty--) {            // 100 % -> 0 % in about 1 s
    ledcWrite(LED_PIN, duty);
    delayMicroseconds(FALL_STEP_US);
  }
}
