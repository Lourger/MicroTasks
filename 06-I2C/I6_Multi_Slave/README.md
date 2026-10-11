# I6: Multi-Slave I2C Communication

One ESP32 reads a DS1307 clock and a BMP180 sensor and shows everything on an SSD1306 OLED.

## The task

One microcontroller is the master for three I2C slaves on the same two wires: a **DS1307** clock (date and time), a **BMP180** sensor (temperature, pressure, altitude) and an **SSD1306** OLED that shows all of it, updated every second.

No extra libraries: the sketch talks to all three devices directly with `Wire`. At start-up it retries each device every 100 ms until it answers, and if the RTC isn't running, it sets it to the time the sketch was compiled.

## Rules

| Slave | Address | Used for |
|---|---|---|
| DS1307 | 0x68 | date and time (24-hour) |
| BMP180 | 0x77 | temperature, pressure (oversampling 3), altitude |
| SSD1306 | 0x3C | row 0: D/M/YYYY + H:MM:SS; then Pressure (hPa), Temp, Altitude |
| Update | every 1000 ms |  |

## Parts

- ESP32 DevKit
- DS1307 RTC module + coin cell
- BMP180 module
- SSD1306 OLED 128 × 64, I2C
- 2 resistors, 4.7 kΩ (if the modules have none)
- jumper wires

## Wiring

| Bus line | ESP32 | DS1307 | BMP180 | SSD1306 |
|---|---|---|---|---|
| SDA | GPIO21 | SDA | SDA | SDA |
| SCL | GPIO22 | SCL | SCL | SCL |
| GND | GND | GND | GND | GND |
| I2C address | - | 0x68 | 0x77 | 0x3C |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

Three slaves, two wires. Each one listens only for its own address. Power: DS1307 from 5 V (VIN), BMP180 and OLED from 3.3 V, all GNDs together.

## Upload and run

Open `I6_Multi_Slave/I6_Multi_Slave.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## What the screen shows

```
11/10/2026    14:05:09

Pressure: 1008.53 hPa

Temp: 27.50 °C

Altitude: 40.12 m
```

Example values. The text sits on every other 8-pixel row (pages 0, 2, 4 and 6), and the time starts at pixel 70.

## One BMP180 reading

1. Write `0x2E` to register 0xF4 → wait 4.5 ms → read 2 bytes from 0xF6: raw temperature
2. Write `0x34 + (3 << 6) = 0xF4` to register 0xF4 → wait 25.5 ms → read 3 bytes: raw pressure
3. Datasheet example: raw 27898 and 23843 (oversampling 0) → `15.0 °C` and `69964 Pa`; this sketch's formulas give exactly these
4. Altitude: `44330 × (1 − (69964 ÷ 101325)^0.1903) ≈ 3017 m`

## Levels and pull-ups

**Pull-ups:** one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, for the whole bus (not one pair per device). Many modules already carry their own pull-ups; if several modules each add a pair, the combined resistance can get too low, so check before adding more.

The DS1307 needs 5 V; if its module pulls SDA and SCL up to 5 V, remove those pull-ups (see task I4).
