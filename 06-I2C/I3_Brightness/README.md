# I3: LED Brightness Control Using I2C

A potentiometer on one ESP32 sets the brightness of an LED on another ESP32 over I2C.

## The task

The **master** reads a potentiometer and sends the value over I2C; the **slave** sets its LED brightness with PWM.

The master sends a 4-byte packet (full scale and reading, each as two bytes, low byte first) to address 0x08 whenever the reading changes; the slave maps it to 12-bit PWM at 1 kHz.

## Rules

| Board | Pins | Does |
|---|---|---|
| Master | pot GPIO34; SDA 21, SCL 22 | on each change: send 4 bytes, print it |
| Slave | LED GPIO2 (150 Ω); SDA 21, SCL 22 | address 0x08; 1 kHz, 12-bit PWM; prints each reading |
| Packet | max_lo max_hi val_lo val_hi | full scale 4095, then the reading |

## Parts

- 2 ESP32 DevKits
- 1 potentiometer, 1 kΩ to 10 kΩ
- 1 LED
- 1 resistor, 150 Ω
- 2 resistors, 4.7 kΩ
- jumper wires

## Wiring

| Bus line | Master | Slave ESP32 |
|---|---|---|
| SDA | GPIO21 | GPIO21 |
| SCL | GPIO22 | GPIO22 |
| GND | GND | GND |
| I2C address | - | address 0x08 |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

The same bus as task I2 (LED Toggling Using I2C): SDA to SDA, SCL to SCL, GND to GND, one pair of pull-ups.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`I3_Brightness_Master`](I3_Brightness_Master/I3_Brightness_Master.ino) | Master ESP32 | 115200 baud |
| [`I3_Brightness_Slave`](I3_Brightness_Slave/I3_Brightness_Slave.ino) | Slave ESP32 | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Pot on the master, LED on the slave

| Pin (on its own board) | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO2 | output | 150 Ω resistor → slave LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Worked example: one packet

| Byte | Meaning | Reading 2048 | Reading 300 |
|---|---|---|---|
| 1 | full scale, low | 0xFF | 0xFF |
| 2 | full scale, high | 0x0F | 0x0F |
| 3 | reading, low | 0x00 | 0x2C |
| 4 | reading, high | 0x08 | 0x01 |

300 = 0x012C: low byte 0x2C, high byte 0x01.
