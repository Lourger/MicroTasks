#include <Arduino.h>
#include "soc/soc.h"                // REG_WRITE()
#include "soc/gpio_reg.h"           // GPIO_OUT_W1TS_REG, GPIO_OUT_W1TC_REG

#define PULSE_PIN     4             // to the counter's GPIO4
#define BUTTON_PIN    15            // button to GND (internal pull-up)
#define PULSE_COUNT   1000          // pulses per press
#define DEBOUNCE_MS   50

int lastReading = HIGH;
int stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeTime = millis();
  }
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS && reading != stableLevel) {
    stableLevel = reading;
    return stableLevel == LOW;
  }
  return false;
}

void sendBurst() {
  const uint32_t mask = 1UL << PULSE_PIN;
  noInterrupts();                   // keep the burst even: nothing else runs meanwhile
  for (int i = 0; i < PULSE_COUNT; i++) {
    REG_WRITE(GPIO_OUT_W1TS_REG, mask);                    // HIGH
    __asm__ __volatile__("nop; nop; nop; nop; nop; nop; nop; nop;");
    REG_WRITE(GPIO_OUT_W1TC_REG, mask);                    // LOW
    __asm__ __volatile__("nop; nop; nop; nop; nop; nop; nop; nop;");
  }
  interrupts();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);
}

void loop() {
  if (buttonPressed()) {
    sendBurst();
    Serial.println("Pulse Generated");
  }
}
