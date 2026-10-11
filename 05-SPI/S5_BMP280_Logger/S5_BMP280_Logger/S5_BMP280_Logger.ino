#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

// ---------- SPI bus (VSPI: SCK 18, MISO 19, MOSI 23) ----------
#define BMP_CS_PIN      5           // BMP280 CSB
#define SD_CS_PIN       4           // SD card CS

// ---------- I2C bus (SDA 21, SCL 22, 100 kHz) ----------
#define RTC_ADDRESS     0x68        // DS1307
#define OLED_ADDRESS    0x3C        // SSD1306, 128 x 64

// ---------- Logging ----------
#define LOG_INTERVAL_MS 1000
#define FILE_NAME       "/data.csv"
#define CSV_HEADER      "Temperature,Pressure,Altitude,Date,Time"
#define SEA_LEVEL_PA    101325.0f   // standard pressure at sea level
#define FORCE_RTC_SET   0           // 1 for one upload: copy the compile time into the RTC

// ---------- BMP280 registers and settings ----------
#define BMP_REG_CALIB   0x88        // 24 bytes of factory calibration
#define BMP_REG_ID      0xD0
#define BMP_REG_CTRL    0xF4        // ctrl_meas
#define BMP_REG_CONFIG  0xF5
#define BMP_REG_DATA    0xF7        // 6 bytes: pressure, then temperature
#define BMP_CHIP_ID     0x58
#define BMP_CONFIG      0x90        // standby 500 ms (100), filter x16 (100), 4-wire SPI (0)
#define BMP_CTRL        0x57        // temperature x2 (010), pressure x16 (101), normal mode (11)

SPISettings bmpSettings(1000000, MSBFIRST, SPI_MODE0);

struct BmpCalibration {
  uint16_t T1;
  int16_t  T2, T3;
  uint16_t P1;
  int16_t  P2, P3, P4, P5, P6, P7, P8, P9;
};
BmpCalibration cal;
int32_t tFine;                      // shared by the temperature and pressure formulas

struct DateTime {
  uint8_t  second, minute, hour, day, month;
  uint16_t year;
};

char dateText[16] = "00-00-0000";   // DD-MM-YYYY, kept if an RTC read fails
char timeText[16] = "00:00:00";     // HH:MM:SS

// ---------- 5 x 7 font: only the characters this logger shows ----------
struct Glyph {
  char    c;
  uint8_t columns[5];               // bit 0 = top pixel
};
const Glyph FONT[] = {
  {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},  // space
  {'-', {0x00, 0x08, 0x08, 0x08, 0x00}},
  {'.', {0x00, 0x60, 0x60, 0x00, 0x00}},
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
  {'e', {0x38, 0x54, 0x54, 0x54, 0x18}},
  {'l', {0x00, 0x41, 0x7F, 0x40, 0x00}},
  {'m', {0x7C, 0x04, 0x18, 0x04, 0x78}},
  {'p', {0x7C, 0x14, 0x14, 0x14, 0x08}},
  {'r', {0x7C, 0x08, 0x04, 0x04, 0x08}},
  {'s', {0x48, 0x54, 0x54, 0x54, 0x20}},
  {'t', {0x04, 0x3F, 0x44, 0x40, 0x20}},
  {'`', {0x06, 0x09, 0x09, 0x06, 0x00}},  // degree sign (drawn for `)
};

// =============================== BMP280 (SPI) ===============================
void bmpRead(uint8_t reg, uint8_t *buffer, size_t length) {
  SPI.beginTransaction(bmpSettings);
  digitalWrite(BMP_CS_PIN, LOW);
  SPI.transfer(reg | 0x80);         // bit 7 = 1: read
  for (size_t i = 0; i < length; i++) {
    buffer[i] = SPI.transfer(0x00); // the address goes up by itself
  }
  digitalWrite(BMP_CS_PIN, HIGH);
  SPI.endTransaction();
}

void bmpWrite(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(bmpSettings);
  digitalWrite(BMP_CS_PIN, LOW);
  SPI.transfer(reg & 0x7F);         // bit 7 = 0: write
  SPI.transfer(value);
  digitalWrite(BMP_CS_PIN, HIGH);
  SPI.endTransaction();
}

uint16_t littleEndian(const uint8_t *b, int i) {
  return b[i] | (b[i + 1] << 8);
}

bool bmpBegin() {
  uint8_t id = 0;
  bmpRead(BMP_REG_ID, &id, 1);
  if (id != BMP_CHIP_ID) {
    return false;
  }
  uint8_t c[24];
  bmpRead(BMP_REG_CALIB, c, 24);
  cal.T1 = littleEndian(c, 0);
  cal.T2 = (int16_t)littleEndian(c, 2);
  cal.T3 = (int16_t)littleEndian(c, 4);
  cal.P1 = littleEndian(c, 6);
  cal.P2 = (int16_t)littleEndian(c, 8);
  cal.P3 = (int16_t)littleEndian(c, 10);
  cal.P4 = (int16_t)littleEndian(c, 12);
  cal.P5 = (int16_t)littleEndian(c, 14);
  cal.P6 = (int16_t)littleEndian(c, 16);
  cal.P7 = (int16_t)littleEndian(c, 18);
  cal.P8 = (int16_t)littleEndian(c, 20);
  cal.P9 = (int16_t)littleEndian(c, 22);

  bmpWrite(BMP_REG_CTRL, 0x00);     // sleep mode first: config writes can be ignored in normal mode
  bmpWrite(BMP_REG_CONFIG, BMP_CONFIG);
  bmpWrite(BMP_REG_CTRL, BMP_CTRL); // normal mode: measures by itself every ~0.55 s
  return true;
}

// Bosch's integer compensation formulas (BMP280 datasheet, section 8.2)
float compensateTemperature(int32_t adcT) {
  int32_t var1 = ((((adcT >> 3) - ((int32_t)cal.T1 << 1))) * ((int32_t)cal.T2)) >> 11;
  int32_t var2 = (((((adcT >> 4) - ((int32_t)cal.T1)) * ((adcT >> 4) - ((int32_t)cal.T1))) >> 12) *
                  ((int32_t)cal.T3)) >> 14;
  tFine = var1 + var2;
  return ((tFine * 5 + 128) >> 8) / 100.0f;                  // degrees C
}

float compensatePressure(int32_t adcP) {
  int64_t var1 = ((int64_t)tFine) - 128000;
  int64_t var2 = var1 * var1 * (int64_t)cal.P6;
  var2 = var2 + ((var1 * (int64_t)cal.P5) << 17);
  var2 = var2 + (((int64_t)cal.P4) << 35);
  var1 = ((var1 * var1 * (int64_t)cal.P3) >> 8) + ((var1 * (int64_t)cal.P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)cal.P1) >> 33;
  if (var1 == 0) {
    return 0;                       // avoid dividing by zero
  }
  int64_t p = 1048576 - adcP;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)cal.P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)cal.P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)cal.P7) << 4);
  return p / 256.0f;                                         // pascals
}

void bmpReadAll(float &temperatureC, float &pressurePa) {
  uint8_t d[6];
  bmpRead(BMP_REG_DATA, d, 6);      // one burst, so all 6 bytes are from the same measurement
  int32_t adcP = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
  int32_t adcT = ((int32_t)d[3] << 12) | ((int32_t)d[4] << 4) | (d[5] >> 4);
  temperatureC = compensateTemperature(adcT);   // first: it sets tFine
  pressurePa   = compensatePressure(adcP);
}

float altitudeMeters(float pressurePa) {
  return 44330.0f * (1.0f - powf(pressurePa / SEA_LEVEL_PA, 0.1903f));
}

// =============================== DS1307 (I2C) ===============================
bool i2cDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;   // 0 = the device answered (ACK)
}

uint8_t bcdToDec(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
uint8_t decToBcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

bool rtcReadRegisters(uint8_t *r) {   // the 7 time registers, 0x00 to 0x06
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x00);                 // start at the seconds register
  if (Wire.endTransmission() != 0) {
    return false;
  }
  if (Wire.requestFrom(RTC_ADDRESS, 7) != 7) {
    return false;
  }
  for (int i = 0; i < 7; i++) {
    r[i] = Wire.read();
  }
  return true;
}

bool rtcRead(DateTime &t) {
  uint8_t r[7];
  if (!rtcReadRegisters(r)) {
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

bool rtcIsHalted() {                // true on a new DS1307 or after its battery was removed
  uint8_t r[7];
  return rtcReadRegisters(r) && (r[0] & 0x80);
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

// =============================== SSD1306 (I2C) ==============================
void oledBegin() {
  static const uint8_t INIT[] = {
    0xAE,         // display off
    0xD5, 0x80,   // clock divider
    0xA8, 0x3F,   // 64 rows
    0xD3, 0x00,   // no vertical offset
    0x40,         // start at RAM row 0
    0x8D, 0x14,   // charge pump on (makes the panel voltage)
    0x20, 0x00,   // horizontal addressing
    0xA1,         // flip left-right
    0xC8,         // flip up-down
    0xDA, 0x12,   // COM pin layout for 128 x 64
    0x81, 0xCF,   // contrast
    0xD9, 0xF1,   // pre-charge period
    0xDB, 0x40,   // VCOMH level
    0xA4,         // show the RAM
    0xA6,         // not inverted
    0xAF          // display on
  };
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x00);                 // control byte: everything after it is a command
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

void oledPrintLine(uint8_t page, const char *text) {   // page 0..7 = one 8-pixel text row
  uint8_t row[128] = {0};           // 21 characters x 6 columns; the rest stays blank
  for (int i = 0; text[i] != '\0' && i < 21; i++) {
    memcpy(&row[i * 6], glyphFor(text[i]), 5);
  }
  const uint8_t AREA[] = {0x21, 0, 127, 0x22, page, page};  // columns 0..127, this page only
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x00);
  Wire.write(AREA, sizeof(AREA));
  Wire.endTransmission();
  for (int i = 0; i < 128; i += 16) {
    Wire.beginTransmission(OLED_ADDRESS);
    Wire.write(0x40);               // control byte: everything after it is pixel data
    Wire.write(&row[i], 16);
    Wire.endTransmission();
  }
}

// =================================== Main ===================================
void setup() {
  Serial.begin(115200);
  pinMode(BMP_CS_PIN, OUTPUT);      // both SPI slaves deselected before anything starts
  digitalWrite(BMP_CS_PIN, HIGH);
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  SPI.begin();
  Wire.begin();

  Serial.print("Starting BMP280");
  while (!bmpBegin()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" OK");
  delay(3000);                      // let the sensor finish its first measurements

  Serial.print("Starting SD card");
  while (!SD.begin(SD_CS_PIN)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" OK");

  Serial.print("Opening " FILE_NAME);
  File file = SD.open(FILE_NAME, FILE_APPEND);
  while (!file) {
    Serial.print(".");
    delay(100);
    file = SD.open(FILE_NAME, FILE_APPEND);
  }
  if (file.size() == 0) {
    file.println(CSV_HEADER);       // new file: write the column names once
  }
  file.close();
  Serial.println(" OK");

  Serial.print("Starting RTC");
  while (!i2cDevicePresent(RTC_ADDRESS)) {
    Serial.print(".");
    delay(100);
  }
  if (FORCE_RTC_SET || rtcIsHalted()) {
    rtcSet(compileTime());
    Serial.print(" (set from compile time)");
  }
  Serial.println(" OK");

  Serial.print("Looking for the OLED");
  while (!i2cDevicePresent(OLED_ADDRESS)) {
    Serial.print(".");
    delay(100);
  }
  oledBegin();
  for (uint8_t page = 0; page < 8; page++) {
    oledPrintLine(page, "");        // clear the screen
  }
  Serial.println(" OK");
}

void loop() {
  // 1) Measure
  float temperatureC, pressurePa;
  bmpReadAll(temperatureC, pressurePa);
  float altitudeM = altitudeMeters(pressurePa);

  // 2) Time stamp
  DateTime now;
  if (rtcRead(now)) {
    snprintf(dateText, sizeof(dateText), "%02d-%02d-%04d", now.day, now.month, now.year);
    snprintf(timeText, sizeof(timeText), "%02d:%02d:%02d", now.hour, now.minute, now.second);
  } else {
    Serial.println("RTC read failed! Using the last time.");
  }

  // 3) Serial Monitor
  Serial.printf("%s   %s\n", dateText, timeText);
  Serial.printf("Temperature: %.2f C\n", temperatureC);
  Serial.printf("Pressure: %.2f Pa\n", pressurePa);
  Serial.printf("Altitude: %.2f m (approx.)\n", altitudeM);

  // 4) OLED: four lines, on every other text row
  char line[40];                   // oledPrintLine() shows the first 21 characters
  snprintf(line, sizeof(line), "%s %s", dateText, timeText);
  oledPrintLine(0, line);
  snprintf(line, sizeof(line), "Temp: %.2f `C", temperatureC);   // ` is drawn as a degree sign
  oledPrintLine(2, line);
  snprintf(line, sizeof(line), "Press: %.2f Pa", pressurePa);
  oledPrintLine(4, line);
  snprintf(line, sizeof(line), "Alt: %.2f m", altitudeM);
  oledPrintLine(6, line);

  // 5) SD card: one CSV row
  char csvRow[96];
  snprintf(csvRow, sizeof(csvRow), "%.2f,%.2f,%.2f,%s,%s",
           temperatureC, pressurePa, altitudeM, dateText, timeText);
  File file = SD.open(FILE_NAME, FILE_APPEND);
  while (!file) {
    Serial.print(".");
    delay(100);
    file = SD.open(FILE_NAME, FILE_APPEND);
  }
  file.println(csvRow);
  file.close();
  Serial.println("Saved to " FILE_NAME);
  Serial.println();

  // 6) Sleep until the next whole second of millis()
  delay(LOG_INTERVAL_MS - millis() % LOG_INTERVAL_MS);
}
