#include <Arduino.h>
#include <SPI.h>

// ---------- SPI pins (VSPI) ----------
#define SPI_SCK_PIN    18
#define SPI_MISO_PIN   19
#define SPI_MOSI_PIN   23
#define SPI_CS_PIN     5            // chip select to the slave, active LOW

// ---------- Button ----------
#define ROLL_BUTTON_PIN 4           // button to GND (internal pull-up)
#define DEBOUNCE_MS     50

// ---------- SPI settings: 4 MHz, MSB first, mode 2 ----------
SPISettings diceSettings(4000000, MSBFIRST, SPI_MODE2);

int lastReading = HIGH;
int stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {              // true once per press
  int reading = digitalRead(ROLL_BUTTON_PIN);
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeTime = millis();
  }
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS && reading != stableLevel) {
    stableLevel = reading;
    if (stableLevel == LOW) {
      return true;
    }
  }
  return false;
}

uint8_t requestRoll() {             // one SPI byte exchange with the slave
  SPI.beginTransaction(diceSettings);
  digitalWrite(SPI_CS_PIN, LOW);
  uint8_t roll = SPI.transfer(0x00);  // send a dummy byte, receive the roll
  digitalWrite(SPI_CS_PIN, HIGH);
  SPI.endTransaction();
  return roll;
}

void setup() {
  Serial.begin(115200);
  pinMode(ROLL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);   // slave not selected
  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
  Serial.println("Press the button to roll the dice.");
}

void loop() {
  if (buttonPressed()) {
    uint8_t roll = requestRoll();
    if (roll >= 1 && roll <= 6) {
      Serial.print("Dice roll: ");
      Serial.println(roll);
    } else {
      Serial.println("No valid roll. Is the slave connected?");
    }
  }
}
