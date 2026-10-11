#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// ---------- Shared SPI bus (VSPI) ----------
#define SD_CS_PIN    5              // shared CS line, 4.7k pull-up to 3.3 V
#define SPI_SCK_PIN  18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23

// ---------- Buttons (to GND, internal pull-ups) ----------
#define INC_BUTTON_PIN   15
#define RESET_BUTTON_PIN 4

#define DEBOUNCE_MS  50
#define FILE_NAME    "/count.txt"

SPIClass spi(VSPI);
uint8_t counter = 0;

// ---------- Debounce state: [0] = INC, [1] = RESET ----------
const uint8_t BUTTON_PINS[2] = {INC_BUTTON_PIN, RESET_BUTTON_PIN};
uint8_t lastReading[2] = {HIGH, HIGH};
uint8_t stableLevel[2] = {HIGH, HIGH};
unsigned long lastChangeTime[2] = {0, 0};

bool buttonPressed(int index) {     // true once per press of button [index]
  uint8_t reading = digitalRead(BUTTON_PINS[index]);
  if (reading != lastReading[index]) {
    lastReading[index]    = reading;
    lastChangeTime[index] = millis();
  }
  if ((millis() - lastChangeTime[index]) >= DEBOUNCE_MS && reading != stableLevel[index]) {
    stableLevel[index] = reading;
    if (stableLevel[index] == LOW) {
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
  pinMode(SPI_MOSI_PIN, INPUT);     // INPUT = high impedance: the other master can drive these
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

void writeCounterToSD(uint8_t value) {
  enableSPI();
  if (SD.exists(FILE_NAME)) {
    SD.remove(FILE_NAME);           // start from an empty file (on the ESP32, FILE_WRITE alone also would)
    Serial.println("Old counter file removed.");
  }
  File file = SD.open(FILE_NAME, FILE_WRITE);
  if (file) {
    file.print(value);
    file.close();
    Serial.println("Counter saved on the card.");
  } else {
    Serial.println("Error: could not write the card.");
  }
  disableSPI();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(INC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Master 1: counter on the SD card (INC and RESET buttons)");
}

void loop() {
  if (buttonPressed(0)) {
    Serial.println("INC pressed.");
    counter = readCounterFromSD();
    Serial.print("Counter was: ");
    Serial.println(counter);
    counter++;
    writeCounterToSD(counter);
    Serial.print("Counter is now: ");
    Serial.println(counter);
  }

  if (buttonPressed(1)) {
    counter = 0;
    writeCounterToSD(counter);
    Serial.println("Counter reset to 0.");
  }
}
