# S4: SPI vs I2C Speed Comparison

Send 100 KB over I2C and over SPI between two ESP32s, and compare the times.

## The task

A master sends 100 KB (1000 bytes, 100 times) to a slave, first over I2C, then over SPI, and prints how long each took. The slave counts the bytes it received over each bus and prints the totals. The master also prints how many times faster SPI was.

The slave receives SPI with the ESP-IDF SPI slave driver and DMA. One transaction is one 1000-byte block, so the master raises CS after each block.

## Rules

| Bus | Settings | Timed |
|---|---|---|
| I2C | 400 kHz, address 9, 32-byte chunks | each endTransmission() |
| SPI | 4 MHz, mode 0, one byte per transfer() | each transfer(); 10 µs gap not timed |
| Slave | prints I2C/SPI data received: 100.000 KB |  |

## Parts

- 2 ESP32 DevKits
- 2 resistors, 4.7 kΩ (I2C pull-ups)
- jumper wires

## Wiring

| Signal | Master | Slave | Direction |
|---|---|---|---|
| SDA | GPIO21 | GPIO21 | both ways |
| SCL | GPIO22 | GPIO22 | Master → Slave |
| SCK | GPIO18 | GPIO18 | Master → Slave |
| MOSI | GPIO23 | GPIO23 | Master → Slave |
| MISO | GPIO19 | GPIO19 | Slave → Master |
| CS | GPIO5 | GPIO5 | Master → Slave |
| GND | GND | GND | common ground |

I2C on 21/22, SPI on 18/19/23/5, and a common GND. SDA carries data both ways (the slave answers with acknowledge bits).

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`S4_Speed_Master`](S4_Speed_Master/S4_Speed_Master.ino) | Master ESP32 | 115200 baud |
| [`S4_Speed_Slave`](S4_Speed_Slave/S4_Speed_Slave.ino) | Slave ESP32 | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## I2C pull-ups

| Bus line | Connection |
|---|---|
| SDA | 4.7 kΩ resistor → 3.3 V |
| SCL | 4.7 kΩ resistor → 3.3 V |

The ESP32's internal pull-ups (about 45 kΩ) are too weak for clean 400 kHz edges. One pair of 4.7 kΩ resistors on the bus is enough.

## Worked example: the fastest possible times

1. I2C sends 9 clocks per byte (8 bits + acknowledge): `9 ÷ 400 kHz = 22.5 µs` per byte
2. `100,000 × 22.5 µs = 2.25 s`, plus an address byte for each of the 3200 chunks
3. SPI sends 8 clocks per byte: `8 ÷ 4 MHz = 2 µs` per byte → `0.2 s` for 100 KB
4. So SPI is about 11 times faster on the wires. Your measured times will be higher, mostly for SPI, because each `transfer()` call adds a little software time to only 2 µs of bus time
