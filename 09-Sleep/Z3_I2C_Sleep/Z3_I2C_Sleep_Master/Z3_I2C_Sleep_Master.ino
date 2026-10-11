#include <Arduino.h>
#include <Wire.h>

// ---------- Pins ----------
#define BUTTON_PIN     15           // button to GND (internal pull-up)
#define I2C_SDA_PIN    21           // 4.7k pull-ups to 3.3 V on SDA and SCL
#define I2C_SCL_PIN    22

// ---------- I2C ----------
#define SLAVE_ADDRESS  0x08
#define WAKE_DELAY_MS  20           // time for the slave to wake and load its reading

#define DEBOUNCE_MS    50

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
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.println("Master ready. Press the button to read the slave.");
}

void loop() {
  if (buttonPressed()) {
    // 1) Wake-up call: the START pulls SDA LOW, which wakes the sleeping slave.
    //    The slave is still asleep during this transfer, so no ACK comes back.
    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.endTransmission();
    delay(WAKE_DELAY_MS);

    // 2) Read the two bytes the slave has prepared: low byte, then high byte
    if (Wire.requestFrom(SLAVE_ADDRESS, 2) == 2) {
      uint8_t low  = Wire.read();
      uint8_t high = Wire.read();
      uint16_t adcValue = low | (high << 8);
      Serial.print("ADC value: ");
      Serial.println(adcValue);
    } else {
      Serial.println("Read failed");
    }
  }
}
