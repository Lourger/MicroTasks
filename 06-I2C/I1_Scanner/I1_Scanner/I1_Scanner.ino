#include <Arduino.h>
#include <Wire.h>

// ---------- I2C pins ----------
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

// ---------- Scan ----------
#define FIRST_ADDRESS  1            // 0x00 is the general call address, not a device
#define LAST_ADDRESS   126          // 0x7E
#define SCAN_EVERY_MS  5000

void setup() {
  Serial.begin(115200);
  delay(1000);                      // time to open the Serial Monitor
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.println();
  Serial.println("ESP32 I2C Scanner");
}

void loop() {
  Serial.println("Scanning the I2C bus...");
  int devicesFound = 0;

  for (uint8_t address = FIRST_ADDRESS; address <= LAST_ADDRESS; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();   // 0 = a device answered with ACK
    if (error == 0) {
      Serial.printf("Device found at address 0x%02X\n", address);
      devicesFound++;
    } else if (error == 4) {
      Serial.printf("Unknown error at address 0x%02X\n", address);
    }
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print("Total devices found: ");
    Serial.println(devicesFound);
    Serial.println("Scan complete.");
  }
  Serial.println();
  delay(SCAN_EVERY_MS);
}
