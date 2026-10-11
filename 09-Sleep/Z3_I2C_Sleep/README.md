# Z3: I2C Slave with Sleep Mode

A sleeping I2C slave is woken by the master, sends its pot reading, and sleeps again.

## The task

An I2C **slave** with a potentiometer stays in a power-saving mode. When a button on the **master** is pressed, the master wakes the slave over I2C; the slave reads its ADC, sends the value, and goes back to sleep. The master prints the value.

## Rules

| Board | Has | Does |
|---|---|---|
| Master | button GPIO15; SDA 21, SCL 22 | on a press: wake-up call, wait 20 ms, read 2 bytes, print ADC value: N |
| Slave | pot GPIO34; SDA 21, SCL 22 | address 0x08; light sleep; SDA LOW wakes it; loads the reading; sleeps again |
| Data | 2 bytes, low byte first |  |

## Parts

- 2 ESP32 DevKits
- 1 push button
- 1 potentiometer
- 2 resistors, 4.7 kΩ
- jumper wires

## Wiring

| Bus line | Master | Slave ESP32 |
|---|---|---|
| SDA | GPIO21 | GPIO21 |
| SCL | GPIO22 | GPIO22 |
| GND | GND | GND |
| I2C address | - | 0x08, sleeps |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

The usual I2C bus. The slave's SDA pin does two jobs: I2C data, and the wake-up signal.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`Z3_I2C_Sleep_Master`](Z3_I2C_Sleep_Master/Z3_I2C_Sleep_Master.ino) | Master ESP32 (button) | 115200 baud |
| [`Z3_I2C_Sleep_Slave`](Z3_I2C_Sleep_Slave/Z3_I2C_Sleep_Slave.ino) | Slave ESP32 (pot, sleeps) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Button and pot

| Pin (on its own board) | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO15 | INPUT_PULLUP | master button → GND |
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

## One press, step by step

| Time | Master | Slave |
|---|---|---|
| 0 ms | wake-up call: START, address 0x08 → no ACK | asleep; SDA LOW wakes it |
| < 1 ms | waits | awake: reads the pot, loads 2 bytes |
| 20 ms | reads 2 bytes | the I2C hardware sends them |
| 21 ms | prints ADC value: N | onRequest() → readingTaken → sleeps again |

## Light sleep vs deep sleep here

Deep sleep would be useless for this: it switches off the I2C hardware and restarts the program on wake-up, and SDA (GPIO21) isn't an RTC pin anyway. Light sleep keeps the I2C slave set up and continues the program where it stopped. The ESP32 chip draws about 0.8 mA in light sleep (on a DevKit, the board's other parts add several mA).
