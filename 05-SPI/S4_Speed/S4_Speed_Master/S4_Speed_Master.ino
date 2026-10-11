#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// ---------- I2C (SDA 21, SCL 22) ----------
#define SLAVE_I2C_ADDRESS 9
#define I2C_CLOCK_HZ      400000     // fast mode
#define I2C_CHUNK         32         // bytes per transmission

// ---------- SPI (VSPI: SCK 18, MISO 19, MOSI 23) ----------
#define SPI_CS_PIN        5
#define SPI_CLOCK_HZ      4000000
#define SPI_BYTE_GAP_US   10         // pause after each byte, not timed
#define SPI_BLOCK_GAP_US  50         // pause after each block, not timed

// ---------- Test data: one 1000-byte block, sent 100 times ----------
#define BLOCK_SIZE        1000
#define BLOCK_COUNT       100

const char MESSAGE[] =
  "I2C and SPI both connect a microcontroller to chips on the same board, but they make "
  "different trade-offs. I2C needs only two wires, SDA for data and SCL for the clock, and every "
  "device on the bus has its own 7-bit address, so one pair of wires can reach many sensors. Each "
  "byte is followed by an acknowledge bit, and the lines are open-drain with pull-up resistors, which "
  "limits the speed: 100 kHz in standard mode, 400 kHz in fast mode, and 1 MHz in fast mode plus. "
  "SPI uses four wires: SCK for the clock, MOSI from master to slave, MISO from slave to master, and "
  "one chip select line per slave. There are no addresses and no acknowledge bits, the outputs are "
  "push-pull, and data moves in both directions at the same time, so SPI clocks often run at "
  "several MHz and sometimes above 40 MHz. The price is more pins and no built-in check that a "
  "slave received anything. Choose I2C when pins are scarce and the data rate is low, such as for "
  "a temperature sensor or a real-time clock. Choose SPI for displays, SD cards and fast converters.";
static_assert(sizeof(MESSAGE) - 1 >= BLOCK_SIZE, "MESSAGE needs at least 1000 characters");

uint32_t sendOverI2C() {            // returns the time spent on the bus, in microseconds
  uint32_t busTimeUs = 0;
  for (int block = 0; block < BLOCK_COUNT; block++) {
    for (int start = 0; start < BLOCK_SIZE; start += I2C_CHUNK) {
      int end = min(start + I2C_CHUNK, BLOCK_SIZE);
      Wire.beginTransmission(SLAVE_I2C_ADDRESS);
      for (int j = start; j < end; j++) {
        Wire.write((uint8_t)MESSAGE[j]);   // only fills the buffer
      }
      uint32_t t0 = micros();
      uint8_t error = Wire.endTransmission();   // the bytes go out on the wires here
      uint32_t t1 = micros();
      if (error != 0) {
        Serial.print("I2C error: ");
        Serial.println(error);
      } else {
        busTimeUs += t1 - t0;
      }
    }
  }
  return busTimeUs;
}

uint32_t sendOverSPI() {            // returns the time spent on the bus, in microseconds
  uint32_t busTimeUs = 0;
  SPI.beginTransaction(SPISettings(SPI_CLOCK_HZ, MSBFIRST, SPI_MODE0));
  for (int block = 0; block < BLOCK_COUNT; block++) {
    digitalWrite(SPI_CS_PIN, LOW);
    for (int j = 0; j < BLOCK_SIZE; j++) {
      uint32_t t0 = micros();
      SPI.transfer((uint8_t)MESSAGE[j]);
      busTimeUs += micros() - t0;
      delayMicroseconds(SPI_BYTE_GAP_US);
    }
    digitalWrite(SPI_CS_PIN, HIGH);  // one block done: the slave finishes one transaction
    delayMicroseconds(SPI_BLOCK_GAP_US);
  }
  SPI.endTransaction();
  return busTimeUs;
}

void setup() {
  Serial.begin(115200);
  delay(1000);                      // start the slave first; this gives it time
  Wire.begin();
  Wire.setClock(I2C_CLOCK_HZ);
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);
  SPI.begin();

  Serial.println("Sending data...");
  uint32_t i2cTimeUs = sendOverI2C();
  Serial.print("I2C transfer time: ");
  Serial.print(i2cTimeUs);
  Serial.println(" us");

  uint32_t spiTimeUs = sendOverSPI();
  Serial.print("SPI transfer time: ");
  Serial.print(spiTimeUs);
  Serial.println(" us");

  if (spiTimeUs > 0) {
    Serial.print("SPI was ");
    Serial.print((float)i2cTimeUs / spiTimeUs, 1);
    Serial.println(" times faster.");
  }
}

void loop() {
  // the test runs once in setup(); press EN to run it again
}
