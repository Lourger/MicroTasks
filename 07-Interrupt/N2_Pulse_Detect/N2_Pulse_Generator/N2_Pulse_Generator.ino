#include <Arduino.h>
#include "soc/soc.h"                // REG_WRITE()
#include "soc/gpio_reg.h"           // GPIO_OUT_W1TS_REG, GPIO_OUT_W1TC_REG

#define PULSE_PIN       4           // to the detector's GPIO4
#define PULSE_EVERY_MS  5000

void setup() {
  Serial.begin(115200);
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);
}

void loop() {
  noInterrupts();                   // nothing may run between the two writes
  REG_WRITE(GPIO_OUT_W1TS_REG, 1UL << PULSE_PIN);   // pin HIGH
  REG_WRITE(GPIO_OUT_W1TC_REG, 1UL << PULSE_PIN);   // pin LOW again, right away
  interrupts();
  Serial.println("Pulse generated");
  delay(PULSE_EVERY_MS);
}
