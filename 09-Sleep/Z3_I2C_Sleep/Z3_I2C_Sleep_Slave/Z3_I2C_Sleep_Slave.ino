#include <Arduino.h>
#include <Wire.h>
#include <driver/gpio.h>
#include <esp_sleep.h>

// ---------- Pins ----------
#define I2C_SDA_PIN      21
#define I2C_SCL_PIN      22
#define POT_PIN          34         // potentiometer wiper (ADC1)

// ---------- I2C and timing ----------
#define MY_ADDRESS       0x08
#define AWAKE_TIMEOUT_MS 200        // longest time to stay awake waiting for the master's read

volatile bool readingTaken = false; // set by onRequest(), read by loop()

// On the classic ESP32 this runs AFTER the master has read the bytes
void onRequest() {
  readingTaken = true;
}

void loadReading() {                // put the 2 bytes into the I2C hardware before the master asks
  uint16_t adcValue = analogRead(POT_PIN);
  uint8_t bytes[2] = {(uint8_t)(adcValue & 0xFF), (uint8_t)(adcValue >> 8)};   // low byte first
  Wire.slaveWrite(bytes, 2);
  Serial.printf("Awake. ADC value ready: %u\n", adcValue);
}

void sleepUntilI2C() {              // light sleep; a START from the master (SDA LOW) wakes us
  Serial.println("Sleeping...");
  Serial.flush();                   // finish sending before the clocks stop
  esp_light_sleep_start();          // returns here, with everything as it was, when SDA goes LOW
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)MY_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN, 0);   // slave at 0x08

  gpio_wakeup_enable((gpio_num_t)I2C_SDA_PIN, GPIO_INTR_LOW_LEVEL);   // SDA LOW = wake up
  esp_sleep_enable_gpio_wakeup();
}

void loop() {
  sleepUntilI2C();                  // 1) sleep until the master's wake-up call

  readingTaken = false;             // 2) awake: prepare the answer
  loadReading();

  unsigned long start = millis();   // 3) wait for the master to read it (or give up)
  while (!readingTaken && millis() - start < AWAKE_TIMEOUT_MS) {
    delay(1);
  }
  Serial.println(readingTaken ? "Reading sent." : "No read request.");
}
