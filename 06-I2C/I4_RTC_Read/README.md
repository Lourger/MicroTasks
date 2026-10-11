# I4: User-Defined RTC Functions for Time Reading

Read a DS1307 clock with your own functions (no RTC library) and print the time every second.

## The task

Connect an RTC (real-time clock) module over I2C and write your own functions to read the time from it, without the module's library. Print the time on the Serial Monitor.

The sketch talks to a DS1307 at 0x68 through a small set of functions (write with retries, read and write registers, start the clock, set the time, read the time) and prints the time in 12-hour format with AM/PM, once per second.

**Setting the clock:** near the top of the sketch, change `SET_CLOCK` from `0` to `1`, and in `setup()` edit the time in the `rtcSetTime(...)` call (seconds, minutes, hours in 24-hour format, day of week, date, month, two-digit year). Upload once, then set `SET_CLOCK` back to `0` and upload again, so that a reset doesn't set the clock back to that time.

## Rules

| Rule | Value |
|---|---|
| RTC | DS1307, address 0x68, 100 kHz |
| Registers | 0x00..0x06: sec, min, hour, day, date, month, year (BCD) |
| Output | 02:25:07 PM 02/09/25 |
| Every | 1000 ms |
| Library | Wire only: no RTC library |

## Parts

- ESP32 DevKit
- DS1307 RTC module + coin cell
- 2 resistors, 4.7 kΩ
- jumper wires

## Wiring

| Bus line | ESP32 | DS1307 |
|---|---|---|
| SDA | GPIO21 | SDA |
| SCL | GPIO22 | SCL |
| GND | GND | GND |
| I2C address | - | 0x68 |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

Power the DS1307 from 5 V (VIN). It needs 4.5 to 5.5 V: below about 1.25 × the coin cell voltage it switches to battery mode and stops answering on I2C.

## Upload and run

Open `I4_RTC_Read/I4_RTC_Read.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## The time registers

| Reg | Holds | Bits |
|---|---|---|
| 0x00 | seconds | bit 7 = CH (clock halt, 1 = stopped); bits 6-0 = BCD 00-59 |
| 0x01 | minutes | BCD 00-59 |
| 0x02 | hours | bit 6 = 1: 12-hour mode (bit 5 = PM, bits 4-0 = 1-12); bit 6 = 0: 24-hour (bits 5-0 = 0-23) |
| 0x03 | day of week | 1-7 (you decide what 1 means) |
| 0x04 | date | BCD 01-31 |
| 0x05 | month | BCD 01-12 |
| 0x06 | year | BCD 00-99 |

## Worked example: BCD

1. BCD (binary-coded decimal) keeps one decimal digit in each 4-bit half of a byte
2. Register 0x01 reads `0x25` = `0010 0101`: tens 2, units 5 → 25 minutes
3. `bcdToDec(0x25) = (0x25 >> 4) × 10 + (0x25 & 0x0F) = 2 × 10 + 5 = 25`
4. Hours `0x14` in 24-hour mode = 14 → printed as `02 PM`

## Levels and pull-ups

Many DS1307 modules pull SDA and SCL up to 5 V. ESP32 pins take at most about 3.6 V, so remove those pull-ups (or use a level shifter) and pull SDA and SCL up to 3.3 V. The DS1307 reads 3.3 V as HIGH (its minimum HIGH level is 2.2 V).
