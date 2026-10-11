# I5: Multi-Master Interface with I2C 16x2 LCD

Two ESP32 masters share one I2C LCD: one shows the uptime, the other a pot reading.

## The task

Two microcontrollers are both I2C **masters** on the same bus, and a 16×2 LCD with a PCF8574 backpack is the shared **slave**. Master 1 shows the time since it started on line 1; Master 2 reads a potentiometer and shows its ADC value on line 2.

Both sketches drive the LCD at 0x27 with a hand-written LCD driver on top of `Wire` (no LCD library): the same start-up commands, 100 kHz, one update per second each, and every line sent in a single transmission. When a write fails because the other master won the bus, the sketch tries again (up to 3 attempts).

## Rules

| Board | Shows | Format |
|---|---|---|
| Master 1 | time since start, row 0 | Uptime:HHH:MM:SS |
| Master 2 | pot on GPIO34, row 1 | POT Value: NNNN |
| Both | SDA 21, SCL 22, 100 kHz, every 1000 ms | LCD at 0x27 |

## Parts

- 2 ESP32 DevKits
- 16×2 LCD with PCF8574 I2C backpack
- 1 potentiometer
- bidirectional I2C level shifter (for a 5 V LCD)
- jumper wires

## Wiring

| Bus line | Master 1 | LCD 16×2 | Master 2 |
|---|---|---|---|
| SDA | GPIO21 | SDA | GPIO21 |
| SCL | GPIO22 | SCL | GPIO22 |
| GND | GND | GND | GND |
| I2C address | - | 0x27 (PCF8574) | - (second master) |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V on the 3.3 V side, once for the whole bus (many level-shifter boards already have them). See *5 V LCD, 3.3 V ESP32* below.

Both masters and the LCD share SDA, SCL and GND. Run the I2C scanner (task I1) first: some backpacks answer at 0x3F instead of 0x27.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`I5_LCD_Master1`](I5_LCD_Master1/I5_LCD_Master1.ino) | Master 1 (uptime) | 115200 baud |
| [`I5_LCD_Master2`](I5_LCD_Master2/I5_LCD_Master2.ino) | Master 2 (potentiometer) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Master 2's potentiometer

| Master 2 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

## 5 V LCD, 3.3 V ESP32

Most 16×2 LCDs need 5 V, and their backpacks pull SDA and SCL up to 5 V, which is too much for ESP32 pins. A PCF8574 powered at 5 V needs at least 3.5 V to see a HIGH, so 3.3 V pull-ups aren't enough either. Use a bidirectional level shifter between the 3.3 V side (both ESP32s) and the 5 V side (the LCD).

## How arbitration picks a winner

I2C lines can only be pulled LOW; a 1 is sent by letting go. If both masters send at once, they compare each bit they send with what they see on SDA. A master that sends 1 but sees 0 has lost: it stops at once, and the other master never notices.

| Bit (MSB first) | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| Master A sends 0xB2 | 1 | 0 | 1 | 1 | 0 | **0** | 1 | 0 |
| Master B sends 0xB4 | 1 | 0 | 1 | 1 | 0 | **1** | 0 | 0 |
| SDA shows | 1 | 0 | 1 | 1 | 0 | **0** |  |  |

At bit 2, B sends 1 but sees 0: B loses and stops. A's byte arrives complete. Here both masters address the same LCD, so they usually part ways inside the data bytes.
