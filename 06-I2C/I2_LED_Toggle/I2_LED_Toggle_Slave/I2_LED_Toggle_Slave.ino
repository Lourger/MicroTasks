#include <Arduino.h>
#include <Wire.h>
#include <driver/gpio.h>

// ---------- Pins ----------
#define LED_PIN        2            // LED + 150 ohm resistor to GND
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

// ---------- I2C ----------
#define MY_ADDRESS     0x08

void onReceive(int howMany) {       // runs each time the master writes to us
  while (Wire.available()) {
    uint8_t command = Wire.read();
    if (command == 1) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Received 1: LED on");
    } else if (command == 0) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Received 0: LED off");
    }                               // any other value is ignored
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);       // off, matching the master's ledState = 0

  Wire.onReceive(onReceive);
  Wire.begin((uint8_t)MY_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN, 0);   // slave at 0x08
  gpio_pullup_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pullup_dis((gpio_num_t)I2C_SCL_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SCL_PIN);
}

void loop() {
  // nothing: the slave only acts when the master sends a byte
}
