#include <Arduino.h>
#include <driver/rtc_io.h>

// ---------- Pins ----------
#define WAKEUP_PIN  33              // button to GND; ext0 wake-up needs an RTC GPIO
#define POT_PIN     34              // potentiometer wiper (ADC1)

void setup() {
  Serial.begin(115200);
  delay(100);                       // let the serial port start
  pinMode(WAKEUP_PIN, INPUT_PULLUP);

  // 1) Why did the chip start? Only a button wake-up reads the pot
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    int adcValue = analogRead(POT_PIN);
    Serial.print("ADC value: ");
    Serial.println(adcValue);
    delay(10);
  }

  // 2) Wait until the button is released: a LOW pin would wake the chip again at once
  while (digitalRead(WAKEUP_PIN) == LOW) {
    delay(10);
  }
  delay(50);                        // let the contacts stop bouncing

  // 3) Sleep until GPIO33 goes LOW
  esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_PIN, 0);   // 0 = wake on LOW
  rtc_gpio_pullup_en((gpio_num_t)WAKEUP_PIN);                // pull-up stays on in deep sleep
  rtc_gpio_pulldown_dis((gpio_num_t)WAKEUP_PIN);
  Serial.println("Entering deep sleep...");
  Serial.flush();                   // finish sending before the UART powers off
  esp_deep_sleep_start();           // never returns: a wake-up restarts the chip at setup()
}

void loop() {
  // never reached: after every wake-up the chip starts again from setup()
}
