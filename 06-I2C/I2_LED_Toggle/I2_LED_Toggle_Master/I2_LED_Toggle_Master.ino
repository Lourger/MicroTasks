#include <Arduino.h>
#include <Wire.h>
#include <driver/gpio.h>

// ---------- Pins ----------
#define BUTTON_PIN     15           // button to GND (internal pull-up)
#define I2C_SDA_PIN    21           // external 4.7k pull-ups to 3.3 V
#define I2C_SCL_PIN    22

// ---------- I2C ----------
#define SLAVE_ADDRESS  0x08

#define DEBOUNCE_MS    50

uint8_t ledState = 0;               // 0 = off, 1 = on: what the slave was last told

int lastReading = HIGH;
int stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {              // true once per press
  int reading = digitalRead(BUTTON_PIN);
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

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);      // master
  gpio_pullup_dis((gpio_num_t)I2C_SDA_PIN);  // use only the external pull-ups
  gpio_pulldown_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pullup_dis((gpio_num_t)I2C_SCL_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SCL_PIN);
}

void loop() {
  if (buttonPressed()) {
    ledState = !ledState;           // 0 -> 1 -> 0 ...

    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.write(ledState);           // one byte: 1 = LED on, 0 = LED off
    uint8_t error = Wire.endTransmission(true);   // true = finish with a STOP

    if (error == 0) {
      Serial.printf("Sent %d: OK\n", ledState);
    } else {
      Serial.printf("Send failed, error %d\n", error);
    }
  }
}
