#include <Arduino.h>
#include <Wire.h>
#include <driver/gpio.h>

// ---------- Pins ----------
#define POT_PIN        34           // potentiometer wiper (ADC1)
#define I2C_SDA_PIN    21           // external 4.7k pull-ups to 3.3 V
#define I2C_SCL_PIN    22

// ---------- I2C and ADC ----------
#define SLAVE_ADDRESS  0x08
#define ADC_MAX        4095         // 12-bit ADC full scale

int lastSentValue = -1;             // -1: nothing sent yet

void sendBrightness(uint16_t value) {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(ADC_MAX & 0xFF);       // byte 1: full scale, low byte
  Wire.write(ADC_MAX >> 8);         // byte 2: full scale, high byte
  Wire.write(value & 0xFF);         // byte 3: reading, low byte
  Wire.write(value >> 8);           // byte 4: reading, high byte
  uint8_t error = Wire.endTransmission(true);

  if (error == 0) {
    Serial.printf("Sent %u: OK\n", value);
  } else {
    Serial.printf("Send failed, error %d\n", error);
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);      // master
  gpio_pullup_dis((gpio_num_t)I2C_SDA_PIN);  // use only the external pull-ups
  gpio_pulldown_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pullup_dis((gpio_num_t)I2C_SCL_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SCL_PIN);
}

void loop() {
  int reading = analogRead(POT_PIN);         // 0..4095
  if (reading != lastSentValue) {            // send only when it changed
    sendBrightness(reading);
    lastSentValue = reading;
  }
  delay(1);
}
