#include <Arduino.h>
#include <Wire.h>

// ---------- I2C bus: one master (this ESP32), three slaves ----------
#define I2C_SDA_PIN     21          // 4.7k pull-ups to 3.3 V on SDA and SCL
#define I2C_SCL_PIN     22
#define RTC_ADDRESS     0x68        // DS1307
#define BMP_ADDRESS     0x77        // BMP180
#define OLED_ADDRESS    0x3C        // SSD1306, 128 x 64

#define UPDATE_EVERY_MS 1000
#define SEA_LEVEL_PA    101325.0f   // standard pressure at sea level

// ---------- BMP180 registers ----------
#define BMP_REG_CALIB   0xAA        // 22 bytes: 11 calibration values, high byte first
#define BMP_REG_ID      0xD0
#define BMP_REG_CTRL    0xF4        // write a command here to start one measurement
#define BMP_REG_DATA    0xF6        // the result: 2 bytes (temperature) or 3 (pressure)
#define BMP_CHIP_ID     0x55
#define BMP_CMD_TEMP    0x2E
#define BMP_CMD_PRESS   0x34        // + (oversampling << 6)
#define BMP_OSS         3           // oversampling 3: 8 samples, ready after 25.5 ms

struct BmpCalibration {
  int16_t  AC1, AC2, AC3;
  uint16_t AC4, AC5, AC6;
  int16_t  B1, B2, MB, MC, MD;
};
BmpCalibration cal;
int32_t b5;                         // shared by the temperature and pressure formulas

struct DateTime {
  uint8_t  second, minute, hour, day, month;
  uint16_t year;
};
DateTime now = {0, 0, 0, 1, 1, 2000};   // kept if an RTC read fails

// ---------- 5 x 7 font: only the characters this display shows ----------
struct Glyph {
  char    c;
  uint8_t columns[5];               // bit 0 = top pixel
};
const Glyph FONT[] = {
  {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},  // space
  {'-', {0x00, 0x08, 0x08, 0x08, 0x00}},
  {'.', {0x00, 0x60, 0x60, 0x00, 0x00}},
  {'/', {0x20, 0x10, 0x08, 0x04, 0x02}},
  {':', {0x00, 0x36, 0x36, 0x00, 0x00}},
  {'0', {0x3E, 0x51, 0x49, 0x45, 0x3E}},
  {'1', {0x00, 0x42, 0x7F, 0x40, 0x00}},
  {'2', {0x42, 0x61, 0x51, 0x49, 0x46}},
  {'3', {0x21, 0x41, 0x45, 0x4B, 0x31}},
  {'4', {0x18, 0x14, 0x12, 0x7F, 0x10}},
  {'5', {0x27, 0x45, 0x45, 0x45, 0x39}},
  {'6', {0x3C, 0x4A, 0x49, 0x49, 0x30}},
  {'7', {0x01, 0x71, 0x09, 0x05, 0x03}},
  {'8', {0x36, 0x49, 0x49, 0x49, 0x36}},
  {'9', {0x06, 0x49, 0x49, 0x29, 0x1E}},
  {'A', {0x7E, 0x09, 0x09, 0x09, 0x7E}},
  {'C', {0x3E, 0x41, 0x41, 0x41, 0x22}},
  {'P', {0x7F, 0x09, 0x09, 0x09, 0x06}},
  {'T', {0x01, 0x01, 0x7F, 0x01, 0x01}},
  {'a', {0x20, 0x54, 0x54, 0x54, 0x78}},
  {'d', {0x38, 0x44, 0x44, 0x48, 0x7F}},
  {'e', {0x38, 0x54, 0x54, 0x54, 0x18}},
  {'h', {0x7F, 0x08, 0x04, 0x04, 0x78}},
  {'i', {0x00, 0x44, 0x7D, 0x40, 0x00}},
  {'l', {0x00, 0x41, 0x7F, 0x40, 0x00}},
  {'m', {0x7C, 0x04, 0x18, 0x04, 0x78}},
  {'p', {0x7C, 0x14, 0x14, 0x14, 0x08}},
  {'r', {0x7C, 0x08, 0x04, 0x04, 0x08}},
  {'s', {0x48, 0x54, 0x54, 0x54, 0x20}},
  {'t', {0x04, 0x3F, 0x44, 0x40, 0x20}},
  {'u', {0x3C, 0x40, 0x40, 0x20, 0x7C}},
  {'`', {0x06, 0x09, 0x09, 0x06, 0x00}},  // degree sign (drawn for `)
};

// ================================ I2C helpers ================================
bool i2cDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;   // 0 = the device answered (ACK)
}

bool i2cWriteRegister(uint8_t device, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(device);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool i2cReadRegisters(uint8_t device, uint8_t reg, uint8_t *buffer, size_t count) {
  Wire.beginTransmission(device);
  Wire.write(reg);                  // point at the first register
  if (Wire.endTransmission() != 0) {
    return false;
  }
  if (Wire.requestFrom((int)device, (int)count) != (int)count) {
    return false;
  }
  for (size_t i = 0; i < count; i++) {
    buffer[i] = Wire.read();        // the register address goes up by itself
  }
  return true;
}

// ================================== BMP180 ===================================
uint16_t bigEndian(const uint8_t *b, int i) {
  return (b[i] << 8) | b[i + 1];
}

bool bmpBegin() {
  uint8_t id = 0;
  if (!i2cReadRegisters(BMP_ADDRESS, BMP_REG_ID, &id, 1) || id != BMP_CHIP_ID) {
    return false;
  }
  uint8_t c[22];
  if (!i2cReadRegisters(BMP_ADDRESS, BMP_REG_CALIB, c, 22)) {
    return false;
  }
  cal.AC1 = (int16_t)bigEndian(c, 0);
  cal.AC2 = (int16_t)bigEndian(c, 2);
  cal.AC3 = (int16_t)bigEndian(c, 4);
  cal.AC4 = bigEndian(c, 6);
  cal.AC5 = bigEndian(c, 8);
  cal.AC6 = bigEndian(c, 10);
  cal.B1  = (int16_t)bigEndian(c, 12);
  cal.B2  = (int16_t)bigEndian(c, 14);
  cal.MB  = (int16_t)bigEndian(c, 16);
  cal.MC  = (int16_t)bigEndian(c, 18);
  cal.MD  = (int16_t)bigEndian(c, 20);
  return true;
}

int32_t bmpReadRawTemperature() {
  i2cWriteRegister(BMP_ADDRESS, BMP_REG_CTRL, BMP_CMD_TEMP);
  delay(5);                         // conversion takes up to 4.5 ms
  uint8_t d[2] = {0, 0};
  i2cReadRegisters(BMP_ADDRESS, BMP_REG_DATA, d, 2);
  return (d[0] << 8) | d[1];
}

int32_t bmpReadRawPressure() {
  i2cWriteRegister(BMP_ADDRESS, BMP_REG_CTRL, BMP_CMD_PRESS + (BMP_OSS << 6));
  delay(26);                        // up to 25.5 ms at oversampling 3
  uint8_t d[3] = {0, 0, 0};
  i2cReadRegisters(BMP_ADDRESS, BMP_REG_DATA, d, 3);
  return (((int32_t)d[0] << 16) | ((int32_t)d[1] << 8) | d[2]) >> (8 - BMP_OSS);
}

// Bosch's integer formulas (BMP180 datasheet, section 3.5)
float compensateTemperature(int32_t ut) {
  int32_t x1 = ((ut - (int32_t)cal.AC6) * (int32_t)cal.AC5) >> 15;
  int32_t x2 = ((int32_t)cal.MC << 11) / (x1 + cal.MD);
  b5 = x1 + x2;
  return ((b5 + 8) >> 4) / 10.0f;                            // degrees C
}

float compensatePressure(int32_t up) {
  int32_t b6 = b5 - 4000;
  int32_t x1 = ((int32_t)cal.B2 * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = ((int32_t)cal.AC2 * b6) >> 11;
  int32_t x3 = x1 + x2;
  int32_t b3 = ((((int32_t)cal.AC1 * 4 + x3) << BMP_OSS) + 2) / 4;
  x1 = ((int32_t)cal.AC3 * b6) >> 13;
  x2 = ((int32_t)cal.B1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  uint32_t b4 = ((uint32_t)cal.AC4 * (uint32_t)(x3 + 32768)) >> 15;
  uint32_t b7 = ((uint32_t)up - b3) * (uint32_t)(50000 >> BMP_OSS);
  int32_t p;
  if (b7 < 0x80000000) {
    p = (b7 * 2) / b4;
  } else {
    p = (b7 / b4) * 2;
  }
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return p + ((x1 + x2 + 3791) >> 4);                        // pascals
}

float altitudeMeters(float pressurePa) {
  return 44330.0f * (1.0f - powf(pressurePa / SEA_LEVEL_PA, 0.1903f));
}

// ================================== DS1307 ===================================
uint8_t bcdToDec(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
uint8_t decToBcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

bool rtcRead(DateTime &t) {
  uint8_t r[7];
  if (!i2cReadRegisters(RTC_ADDRESS, 0x00, r, 7)) {
    return false;
  }
  t.second = bcdToDec(r[0] & 0x7F); // bit 7 is CH (clock halt)
  t.minute = bcdToDec(r[1]);
  t.hour   = bcdToDec(r[2] & 0x3F); // 24-hour mode
  t.day    = bcdToDec(r[4]);        // r[3] is the day of the week, not used
  t.month  = bcdToDec(r[5]);
  t.year   = 2000 + bcdToDec(r[6]);
  return true;
}

bool rtcIsRunning() {               // false on a new DS1307 or after its battery was removed
  uint8_t seconds = 0x80;
  i2cReadRegisters(RTC_ADDRESS, 0x00, &seconds, 1);
  return (seconds & 0x80) == 0;
}

void rtcSet(const DateTime &t) {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x00);
  Wire.write(decToBcd(t.second));   // CH = 0: the clock runs
  Wire.write(decToBcd(t.minute));
  Wire.write(decToBcd(t.hour));     // bit 6 = 0: 24-hour mode
  Wire.write(1);                    // day of the week, not used
  Wire.write(decToBcd(t.day));
  Wire.write(decToBcd(t.month));
  Wire.write(decToBcd(t.year - 2000));
  Wire.endTransmission();
}

DateTime compileTime() {            // __DATE__ = "Oct 11 2026", __TIME__ = "14:05:09"
  const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char m[4] = {__DATE__[0], __DATE__[1], __DATE__[2], 0};
  DateTime t;
  t.month  = (strstr(months, m) - months) / 3 + 1;
  t.day    = atoi(__DATE__ + 4);
  t.year   = atoi(__DATE__ + 7);
  t.hour   = atoi(__TIME__);
  t.minute = atoi(__TIME__ + 3);
  t.second = atoi(__TIME__ + 6);
  return t;
}

// ================================== SSD1306 ==================================
void oledBegin() {
  static const uint8_t INIT[] = {
    0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14, 0x20, 0x00,
    0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
  };                                // same start-up list as the BMP280 logger
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x00);                 // control byte: the rest are commands
  Wire.write(INIT, sizeof(INIT));
  Wire.endTransmission();
}

const uint8_t *glyphFor(char c) {
  for (const Glyph &g : FONT) {
    if (g.c == c) {
      return g.columns;
    }
  }
  return FONT[0].columns;           // unknown character: blank
}

void drawText(uint8_t *row, int x, const char *text) {   // into a 128-column page buffer
  for (int i = 0; text[i] != '\0' && x + 5 <= 128; i++, x += 6) {
    memcpy(&row[x], glyphFor(text[i]), 5);
  }
}

void oledSendPage(uint8_t page, const uint8_t *row) {    // page 0..7 = one 8-pixel band
  const uint8_t AREA[] = {0x21, 0, 127, 0x22, page, page};
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x00);
  Wire.write(AREA, sizeof(AREA));
  Wire.endTransmission();
  for (int i = 0; i < 128; i += 16) {
    Wire.beginTransmission(OLED_ADDRESS);
    Wire.write(0x40);               // control byte: the rest are pixels
    Wire.write(&row[i], 16);
    Wire.endTransmission();
  }
}

void oledLine(uint8_t page, const char *text) {          // one full line of text
  uint8_t row[128] = {0};
  drawText(row, 0, text);
  oledSendPage(page, row);
}

// =================================== Main ====================================
void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  Serial.print("BMP180 initializing...");
  while (!bmpBegin()) {
    Serial.print(" .");
    delay(100);
  }
  Serial.println("\nBMP180 ready.");

  Serial.print("RTC initializing...");
  while (!i2cDevicePresent(RTC_ADDRESS)) {
    Serial.print(" .");
    delay(100);
  }
  Serial.println("\nRTC ready.");
  if (!rtcIsRunning()) {
    Serial.println("RTC was not running: setting it to the compile time.");
    rtcSet(compileTime());
  }

  Serial.print("OLED initializing...");
  while (!i2cDevicePresent(OLED_ADDRESS)) {
    Serial.print(" .");
    delay(100);
  }
  oledBegin();
  for (uint8_t page = 0; page < 8; page++) {
    oledLine(page, "");             // clear
  }
  Serial.println("\nOLED ready.");
  delay(2000);
}

void loop() {
  // 1) Read the two sensors
  rtcRead(now);                     // on failure, the last time stays in "now"
  float temperatureC = compensateTemperature(bmpReadRawTemperature());   // first: sets b5
  float pressurePa   = compensatePressure(bmpReadRawPressure());
  float altitudeM    = altitudeMeters(pressurePa);

  // 2) Row 0: date at x = 0, time at x = 70
  char text[32];
  uint8_t row[128] = {0};
  snprintf(text, sizeof(text), "%d/%d/%d", now.day, now.month, now.year);
  drawText(row, 0, text);
  snprintf(text, sizeof(text), "%d:%02d:%02d", now.hour, now.minute, now.second);
  drawText(row, 70, text);
  oledSendPage(0, row);

  // 3) The three values on pages 2, 4, 6
  snprintf(text, sizeof(text), "Pressure: %.2f hPa", pressurePa / 100.0f);
  oledLine(2, text);
  snprintf(text, sizeof(text), "Temp: %.2f `C", temperatureC);   // ` is drawn as a degree sign
  oledLine(4, text);
  snprintf(text, sizeof(text), "Altitude: %.2f m", altitudeM);
  oledLine(6, text);

  delay(UPDATE_EVERY_MS);
}
