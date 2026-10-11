#include <Arduino.h>
#include <Wire.h>

// ---------- I2C ----------
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22
#define I2C_CLOCK_HZ   100000

// ---------- LCD (16 x 2, HD44780 behind a PCF8574 I2C backpack) ----------
#define LCD_ADDRESS    0x27         // 0x3F on some backpacks: run the I2C scanner
#define LCD_RS         0x01         // PCF8574 P0: 0 = command, 1 = character
#define LCD_EN         0x04         // PCF8574 P2: the LCD reads data when EN goes LOW
#define LCD_BACKLIGHT  0x08         // PCF8574 P3: backlight on
                                    // P4..P7 carry the 4 data bits D4..D7
#define LCD_RETRIES    3            // try again if the other master won the bus

// Adds one 4-bit half to the open transmission: set data, EN HIGH, EN LOW
void queueNibble(uint8_t nibble, uint8_t rs) {
  uint8_t out = (nibble << 4) | rs | LCD_BACKLIGHT;
  Wire.write(out);                  // data and RS settle first
  Wire.write(out | LCD_EN);         // EN HIGH
  Wire.write(out);                  // EN LOW: the LCD latches the 4 bits
}

void queueByte(uint8_t value, uint8_t rs) {   // 8 bits = high half, then low half
  queueNibble(value >> 4, rs);
  queueNibble(value & 0x0F, rs);
}

void lcdCommand(uint8_t command) {
  Wire.beginTransmission(LCD_ADDRESS);
  queueByte(command, 0);
  Wire.endTransmission();
}

void lcdInit() {
  delay(50);                        // LCD power-up time
  const uint8_t wakeUp[] = {0x03, 0x03, 0x03, 0x02};   // 3 x "8-bit mode", then "4-bit mode"
  for (uint8_t nibble : wakeUp) {
    Wire.beginTransmission(LCD_ADDRESS);
    queueNibble(nibble, 0);
    Wire.endTransmission();
    delay(5);
  }
  lcdCommand(0x28);                 // 4-bit, 2 lines, 5x8 dots
  lcdCommand(0x08);                 // display off
  lcdCommand(0x01);                 // clear
  delay(2);                         // clearing takes up to 1.52 ms
  lcdCommand(0x06);                 // cursor moves right after each character
  lcdCommand(0x0C);                 // display on, cursor hidden
}

// Cursor position and all characters in ONE transmission, so the other
// master can't slip its bytes in between them
bool lcdWriteAt(uint8_t row, uint8_t col, const char *text) {
  Wire.beginTransmission(LCD_ADDRESS);
  queueByte(0x80 | ((row ? 0x40 : 0x00) + col), 0);   // "set cursor" command
  for (int i = 0; text[i] != '\0' && i < 16; i++) {
    queueByte(text[i], LCD_RS);
  }
  return Wire.endTransmission() == 0;
}

void lcdShow(uint8_t row, const char *text) {   // write, and retry if the bus was lost
  for (int attempt = 0; attempt < LCD_RETRIES; attempt++) {
    if (lcdWriteAt(row, 0, text)) {
      return;
    }
    delay(random(2, 10));           // random wait, so both masters don't retry together
  }
  Serial.println("LCD write failed");
}

// ---------- Master 2: potentiometer on row 1 ----------
#define POT_PIN         34
#define UPDATE_EVERY_MS 1000

unsigned long lastUpdate = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  analogReadResolution(12);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_HZ);
  Serial.println("Initializing LCD...");
  lcdInit();
}

void loop() {
  if (millis() - lastUpdate >= UPDATE_EVERY_MS) {
    lastUpdate = millis();
    int potValue = analogRead(POT_PIN);   // 0..4095
    char line[24];                  // lcdWriteAt() shows the first 16 characters
    snprintf(line, sizeof(line), "POT Value: %04d ", potValue);   // trailing space: 16 chars
    lcdShow(1, line);
  }
}
