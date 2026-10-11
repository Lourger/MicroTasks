# I1: I2C Scanner

Find every device on the I2C bus and print its address in hex.

## The task

Write an I2C scanner: try every address on the bus and report which ones answer, in hexadecimal, plus the total number of devices found.

The scanner uses SDA on GPIO21 and SCL on GPIO22, tries addresses 1 to 126 (0x01 to 0x7E), starts a new scan every 5 s, and reports "unknown error" for error code 4.

## Rules

| Rule | Value |
|---|---|
| Pins | SDA 21, SCL 22, 4.7 kΩ pull-ups to 3.3 V |
| Addresses | 0x01 to 0x7E |
| Found | Device found at address 0x3C |
| Summary | Total devices found: N or No I2C devices found. |
| Repeat | every 5000 ms |

## Parts

- ESP32 DevKit
- any I2C modules to find (OLED, RTC, sensor…)
- 2 resistors, 4.7 kΩ (if the modules have no pull-ups)
- jumper wires

## Wiring

| Bus line | ESP32 | module A | module B |
|---|---|---|---|
| SDA | GPIO21 | SDA | SDA |
| SCL | GPIO22 | SCL | SCL |
| GND | GND | GND | GND |
| I2C address | - | e.g. 0x3C | e.g. 0x68 |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

Every module connects to the same two wires, plus 3.3 V (or its own supply) and GND. No module needs its own pin on the ESP32.

## Upload and run

Open `I1_Scanner/I1_Scanner.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## How one address is tested

For each address, the master sends START, the 7-bit address with the write bit, and then STOP, with no data. A device with that address pulls SDA LOW for one clock: the **ACK** (acknowledge). No device means nobody pulls SDA LOW: a **NACK**.

| endTransmission() returns | Means (ESP32 core 3.x) |
|---|---|
| 0 | ACK: a device is at this address |
| 2 | NACK: nobody here (the ESP32 core uses 2 for every NACK) |
| 4 | other error (for example a bus problem) |
| 5 | timeout (for example SDA or SCL stuck LOW) |

**Pull-ups:** one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, for the whole bus (not one pair per device). Many modules already carry their own pull-ups; if several modules each add a pair, the combined resistance can get too low, so check before adding more.
