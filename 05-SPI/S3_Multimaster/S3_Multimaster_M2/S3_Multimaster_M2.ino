#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// ---------- Shared SPI bus (VSPI) ----------
#define SD_CS_PIN    5              // shared CS line, 4.7k pull-up to 3.3 V
#define SPI_SCK_PIN  18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23

// ---------- Button (to GND, internal pull-up) ----------
#define READ_BUTTON_PIN 15

#define DEBOUNCE_MS  50
#define FILE_NAME    "/count.txt"

SPIClass spi(VSPI);

uint8_t lastReading = HIGH;
uint8_t stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {              // true once per press
  uint8_t reading = digitalRead(READ_BUTTON_PIN);
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

void enableSPI() {                  // take the bus
  Serial.println("Taking the SPI bus...");
  pinMode(SD_CS_PIN, INPUT);
  while (digitalRead(SD_CS_PIN) == LOW) {   // LOW: the other master is talking to the card
    Serial.print(".");
    delay(1000);
  }
  spi.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SD_CS_PIN);
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  Serial.print("Starting SD card");
  while (!SD.begin(SD_CS_PIN, spi)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" OK. Bus taken.");
}

void disableSPI() {                 // give the bus back
  SD.end();
  spi.end();
  pinMode(SPI_MOSI_PIN, INPUT);     // INPUT = high impedance
  pinMode(SPI_MISO_PIN, INPUT);
  pinMode(SPI_SCK_PIN, INPUT);
  pinMode(SD_CS_PIN, INPUT);
  Serial.println("Bus released (pins in Hi-Z).");
}

uint8_t readCounterFromSD() {
  int value = 0;
  enableSPI();
  if (SD.exists(FILE_NAME)) {
    File file = SD.open(FILE_NAME, FILE_READ);
    if (file) {
      value = file.parseInt();
      file.close();
      Serial.println("Counter read from the card.");
    } else {
      Serial.println("Error: could not open the file to read.");
    }
  } else {
    Serial.println("No counter file yet. Counter = 0.");
  }
  disableSPI();
  return (uint8_t)value;
}

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(READ_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Master 2: reads the counter from the SD card (READ button)");
}

void loop() {
  if (buttonPressed()) {
    Serial.println("READ pressed.");
    uint8_t stored = readCounterFromSD();
    Serial.print("Stored count is: ");
    Serial.println(stored);
  }
}
