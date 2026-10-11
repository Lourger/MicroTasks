#include <Arduino.h>
#include <Wire.h>
#include <driver/gpio.h>

// ---------- Pins ----------
#define LED_PIN        2            // LED + 150 ohm resistor to GND
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

// ---------- I2C and PWM ----------
#define MY_ADDRESS     0x08
#define PACKET_SIZE    4            // full scale (2 bytes) + reading (2 bytes)
#define PWM_FREQ_HZ    1000
#define PWM_BITS       12           // duty 0..4095
#define PWM_MAX        4095

void onReceive(int howMany) {       // runs each time the master writes to us
  if (howMany != PACKET_SIZE) {     // not our 4-byte packet: throw it away
    while (Wire.available()) {
      Wire.read();
    }
    return;
  }
  uint8_t b[PACKET_SIZE];
  for (int i = 0; i < PACKET_SIZE; i++) {
    b[i] = Wire.read();
  }
  uint16_t adcMax   = b[0] | (b[1] << 8);   // low byte first
  uint16_t adcValue = b[2] | (b[3] << 8);

  if (adcMax > 0) {                 // avoid dividing by zero inside map()
    uint32_t duty = map(adcValue, 0, adcMax, 0, PWM_MAX);
    ledcWrite(LED_PIN, duty);
    Serial.println(adcValue);
  }
}

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_BITS);

  Wire.onReceive(onReceive);
  Wire.begin((uint8_t)MY_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN, 0);   // slave at 0x08
  gpio_pullup_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SDA_PIN);
  gpio_pullup_dis((gpio_num_t)I2C_SCL_PIN);
  gpio_pulldown_dis((gpio_num_t)I2C_SCL_PIN);
}

void loop() {
  // nothing: the slave only acts when the master sends a packet
}
