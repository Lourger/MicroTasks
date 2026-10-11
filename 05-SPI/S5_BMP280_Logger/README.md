# S5: BMP280 Data Logger

Log temperature, pressure and altitude with date and time to an SD card, and show them on an OLED.

## The task

Build a data logger with four modules on two buses:

- **SPI:** a BMP280 sensor (temperature, pressure, and altitude worked out from pressure) and an SD card.
- **I2C:** a DS1307 real-time clock (date and time) and a 128 × 64 SSD1306 OLED.

Every second, one row goes into `data.csv` on the card, and the OLED shows the date, time and the three values.

No extra libraries: the sketch talks to the BMP280, DS1307 and SSD1306 directly with the core's `SPI` and `Wire` libraries, and writes the card with the core's `SD` library.

## Rules

| Part | Bus / pins | Settings |
|---|---|---|
| BMP280 | SPI, CS GPIO5 | normal mode, T ×2, P ×16, filter ×16, standby 500 ms |
| SD card | SPI, CS GPIO4 | /data.csv: Temperature,Pressure,Altitude,Date,Time |
| DS1307 | I2C 0x68 | 24-hour; set from compile time if halted |
| SSD1306 | I2C 0x3C | 4 lines: date time, Temp, Press, Alt |
| Interval | 1000 ms | aligned to whole seconds of millis() |

## Parts

- ESP32 DevKit
- BMP280 module (SPI pins broken out)
- microSD card module + card (FAT32)
- DS1307 RTC module + coin cell
- SSD1306 OLED 128 × 64, I2C
- 2 resistors, 4.7 kΩ
- jumper wires

## Wiring

| Signal | BMP280 | ESP32 | SD card | Direction |
|---|---|---|---|---|
| SCK | SCL | GPIO18 | SCK | ESP32 → BMP280; ESP32 → SD card |
| MOSI | SDA | GPIO23 | MOSI | ESP32 → BMP280; ESP32 → SD card |
| MISO | SDO | GPIO19 | MISO | BMP280 → ESP32; SD card → ESP32 |
| CS | CSB | GPIO5 | - | ESP32 → BMP280 |
| CS | - | GPIO4 | CS | ESP32 → SD card |

The SPI side. On a BMP280 board the SPI pins carry I2C names: SCL is the clock, SDA is data in (MOSI), SDO is data out (MISO), CSB is chip select.

## Upload and run

Open `S5_BMP280_Logger/S5_BMP280_Logger.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## The I2C side

| Signal | DS1307 | ESP32 | OLED | Direction |
|---|---|---|---|---|
| SDA | SDA | GPIO21 | SDA | both ways |
| SCL | SCL | GPIO22 | SCL | ESP32 → DS1307; ESP32 → OLED |

Plus one 4.7 kΩ pull-up from SDA to 3.3 V and one from SCL to 3.3 V.

## Power and levels

| Module | Power | Watch out |
|---|---|---|
| BMP280 | 3.3 V | use a board that brings out all 6 pins (SPI needs CSB and SDO) |
| SD card | 3.3 V (or 5 V for a module with a regulator) | some modules never release MISO, which blocks the BMP280; pick one that does |
| DS1307 | 5 V (VIN pin) | needs 4.5 to 5.5 V; many modules pull SDA/SCL up to 5 V: remove those pull-ups, because ESP32 pins take at most 3.6 V. The DS1307 reads 3.3 V as HIGH. |
| OLED | 3.3 V |  |
| all |  | connect every GND together |

## The settings bytes

1. config `0xF5`: standby 500 ms = `100`, filter ×16 = `100`, 4-wire SPI = `0` → `100 100 0 0 = 0x90`
2. ctrl_meas `0xF4`: temperature ×2 = `010`, pressure ×16 = `101`, normal mode = `11` → `010 101 11 = 0x57`
3. On SPI, bit 7 of the address byte means read (1) or write (0). The BMP280's registers are all 0x80 and up, so a read sends the address as is (`0xD0`, answer `0x58`), and a write clears bit 7 (`0xF4 & 0x7F = 0x74`)

## What the card gets

```
Temperature,Pressure,Altitude,Date,Time
27.53,100845.12,40.12,11-10-2026,14:05:09
27.54,100845.37,40.10,11-10-2026,14:05:10
```

Example values. Altitude is worked out from pressure against the standard sea-level pressure (101325 Pa), so weather changes move it by tens of metres. It's a rough guide.
