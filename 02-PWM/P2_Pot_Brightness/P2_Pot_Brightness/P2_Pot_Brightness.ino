#include <Arduino.h>

// ---------- Hardware ----------
#define LED_PIN        14       // GPIO14 -> 150 ohm -> LED -> GND
#define POT_PIN        34       // potentiometer wiper (ends to 3.3 V and GND)

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    1000     // 1 kHz
#define PWM_RES_BITS   12       // 12-bit duty: 0..4095, the same range as the 12-bit ADC

void setup() {
  analogReadResolution(12);                           // ADC result: 0..4095
  ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcWrite(LED_PIN, 0);                              // start dark
}

void loop() {
  int potValue = analogRead(POT_PIN);                 // 0 (0 V) .. 4095 (about 3.3 V)
  ledcWrite(LED_PIN, potValue);                       // same range, so use it directly as the duty
  delay(1);
}
