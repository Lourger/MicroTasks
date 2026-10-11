#include <Arduino.h>

// ---------- Pins ----------
#define POT_PIN        34           // potentiometer wiper (ADC1)

// ---------- Sleep ----------
#define SLEEP_SECONDS  60
#define US_PER_SECOND  1000000ULL   // the sleep timer counts microseconds

void setup() {
  Serial.begin(115200);
  delay(100);                       // let the serial port start

  // 1) The work for this minute: one reading
  int adcValue = analogRead(POT_PIN);
  Serial.print("ADC value: ");
  Serial.println(adcValue);

  // 2) Sleep for 60 s; the RTC timer then restarts the chip at setup()
  esp_sleep_enable_timer_wakeup(SLEEP_SECONDS * US_PER_SECOND);
  Serial.printf("Entering deep sleep for %d seconds...\n", SLEEP_SECONDS);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // never reached: after every wake-up the chip starts again from setup()
}
