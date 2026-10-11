#include <Arduino.h>
#include <EEPROM.h>

// ---------- Storage ----------
#define EEPROM_SIZE      64
#define COUNTER_ADDRESS  0           // one byte: counts 0..255, then wraps to 0

// ---------- Timing ----------
#define START_DELAY_MS   3000        // time to open the Serial Monitor
#define STATUS_EVERY_MS  5000

uint8_t restartCount = 0;

void setup() {
  delay(START_DELAY_MS);
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  // 1) Count this start
  restartCount = EEPROM.read(COUNTER_ADDRESS) + 1;    // a fresh EEPROM holds 0xFF, so the first count is 0
  EEPROM.write(COUNTER_ADDRESS, restartCount);
  EEPROM.commit();

  Serial.println("=== Restart Counter ===");
  Serial.print("Restart count: ");
  Serial.println(restartCount);

  // 2) Offer a reset (waits until one character arrives)
  Serial.println("Reset the counter to 0? Type Y or N:");
  while (Serial.available() == 0) {
  }
  char answer = Serial.read();

  if (answer == 'Y' || answer == 'y') {
    restartCount = 0;
    EEPROM.write(COUNTER_ADDRESS, restartCount);
    EEPROM.commit();
    Serial.println("Counter reset to 0. Press EN (or power-cycle) to count again.");
  } else {
    Serial.println("Counter not reset.");
  }
}

void loop() {
  Serial.print("Restart count: ");
  Serial.println(restartCount);
  delay(STATUS_EVERY_MS);
}
