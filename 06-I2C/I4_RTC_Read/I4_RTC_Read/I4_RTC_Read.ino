#include <Arduino.h>
#include <Wire.h>

// ---------- I2C ----------
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22
#define I2C_CLOCK_HZ   100000       // the DS1307 supports 100 kHz only
#define RTC_ADDRESS    0x68         // DS1307

// ---------- DS1307 registers ----------
#define REG_SECONDS    0x00         // bit 7 = CH (clock halt)
#define TIME_REGISTERS 7            // 0x00..0x06: sec, min, hour, day of week, date, month, year

#define READ_EVERY_MS  1000
#define SET_CLOCK      0            // 1 for one upload: write the time in setup(), then back to 0

struct RtcTime {
  uint8_t second, minute;
  uint8_t hour12;                   // 1..12
  bool    pm;
  uint8_t dayOfWeek;                // 1..7, meaning is up to you
  uint8_t date, month, year;        // year = 0..99 (20YY)
};

uint8_t bcdToDec(uint8_t v) { return ((v >> 4) * 10) + (v & 0x0F); }
uint8_t decToBcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

// ---------- Low level: bytes on the bus ----------
bool busWrite(uint8_t device, const uint8_t *data, size_t length, int retries = 2) {
  for (int attempt = 0; attempt <= retries; attempt++) {
    Wire.beginTransmission(device);
    Wire.write(data, length);
    if (Wire.endTransmission(true) == 0) {
      return true;                  // ACK on every byte
    }
    delay(2);                       // no ACK: wait a moment and try again
  }
  return false;
}

bool rtcWriteRegisters(uint8_t startReg, const uint8_t *values, size_t count) {
  if (count > 8) {
    return false;
  }
  uint8_t frame[9];
  frame[0] = startReg;              // first byte = register address
  memcpy(&frame[1], values, count); // then the values; the address goes up by itself
  return busWrite(RTC_ADDRESS, frame, count + 1);
}

bool rtcReadRegisters(uint8_t startReg, uint8_t *values, size_t count) {
  if (!busWrite(RTC_ADDRESS, &startReg, 1)) {   // set the register pointer
    return false;
  }
  if (Wire.requestFrom(RTC_ADDRESS, (int)count) != (int)count) {
    return false;                   // short read
  }
  for (size_t i = 0; i < count; i++) {
    values[i] = Wire.read();
  }
  return true;
}

// ---------- RTC functions ----------
bool rtcStartClock() {              // clear CH so the oscillator runs; keep the seconds
  uint8_t seconds;
  if (!rtcReadRegisters(REG_SECONDS, &seconds, 1)) {
    return false;
  }
  seconds &= 0x7F;
  return rtcWriteRegisters(REG_SECONDS, &seconds, 1);
}

bool rtcSetTime(uint8_t sec, uint8_t min, uint8_t hour24, uint8_t dayOfWeek,
                uint8_t date, uint8_t month, uint8_t year2) {
  uint8_t r[TIME_REGISTERS] = {
    decToBcd(sec),                  // CH = 0: clock runs
    decToBcd(min),
    decToBcd(hour24),               // bit 6 = 0: 24-hour mode
    decToBcd(dayOfWeek),
    decToBcd(date),
    decToBcd(month),
    decToBcd(year2)
  };
  return rtcWriteRegisters(REG_SECONDS, r, TIME_REGISTERS);
}

bool rtcGetTime(RtcTime &t) {
  uint8_t r[TIME_REGISTERS];
  if (!rtcReadRegisters(REG_SECONDS, r, TIME_REGISTERS)) {
    return false;
  }
  t.second = bcdToDec(r[0] & 0x7F);  // drop CH
  t.minute = bcdToDec(r[1] & 0x7F);
  if (r[2] & 0x40) {                 // hours register: bit 6 set = 12-hour mode, bit 5 = PM
    t.hour12 = bcdToDec(r[2] & 0x1F);
    t.pm     = r[2] & 0x20;
  } else {                           // stored in 24-hour mode: convert for display
    uint8_t hour24 = bcdToDec(r[2] & 0x3F);
    t.pm     = hour24 >= 12;
    t.hour12 = hour24 % 12;
    if (t.hour12 == 0) {
      t.hour12 = 12;                 // 00:xx -> 12 AM, 12:xx -> 12 PM
    }
  }
  t.dayOfWeek = bcdToDec(r[3] & 0x07);
  t.date      = bcdToDec(r[4] & 0x3F);
  t.month     = bcdToDec(r[5] & 0x1F);
  t.year      = bcdToDec(r[6]);
  return true;
}

void printTime(const RtcTime &t) {   // HH:MM:SS AM  DD/MM/YY
  Serial.printf("%02u:%02u:%02u %s  %02u/%02u/%02u\n",
                t.hour12, t.minute, t.second, t.pm ? "PM" : "AM",
                t.date, t.month, t.year);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_HZ);

  if (SET_CLOCK) {
    rtcSetTime(0, 25, 14, 2, 2, 9, 25);   // 14:25:00, day 2, 02/09/25: edit before use
  }
  if (!rtcStartClock()) {
    Serial.println("RTC start failed");
  }
}

void loop() {
  RtcTime now;
  if (rtcGetTime(now)) {
    printTime(now);
  } else {
    Serial.println("RTC read error");
  }
  delay(READ_EVERY_MS);
}
