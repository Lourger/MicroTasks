#include <Arduino.h>
#include <SPI.h>

// ---------- Pins ----------
#define TOGGLE_BUTTON_PIN 15        // button to GND (internal pull-up)
// SPI uses the VSPI defaults: SCK 18, MISO 19, MOSI 23, SS 5

// ---------- Commands ----------
#define CMD_LED_OFF  0x00
#define CMD_LED_ON   0x01

#define DEBOUNCE_MS  50

bool ledOn = true;                  // the slave starts with its LED on

int lastReading = HIGH;
int stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {              // true once per press
  int reading = digitalRead(TOGGLE_BUTTON_PIN);
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

void sendCommand(uint8_t command) {
  digitalWrite(SS, LOW);            // select the slave
  SPI.transfer(command);
  digitalWrite(SS, HIGH);           // release it
}

void setup() {
  Serial.begin(115200);
  pinMode(TOGGLE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.begin();                      // default settings: 1 MHz, MSB first, mode 0
  Serial.println("SPI master ready. Press the button to toggle the slave's LED.");
}

void loop() {
  if (buttonPressed()) {
    ledOn = !ledOn;
    uint8_t command = ledOn ? CMD_LED_ON : CMD_LED_OFF;
    sendCommand(command);
    Serial.printf("Sent: 0x%02X\n", command);
  }
}
