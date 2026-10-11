# S2: LED Toggle Using SPI

A button on one ESP32 toggles an LED on another ESP32 over SPI.

## The task

Connect two microcontrollers with SPI: the **master** has a push button, the **slave** has an LED. Each press toggles the LED.

Both boards are ESP32s. The master sends `0x01` (on) or `0x00` (off) with the Arduino `SPI` library; the slave uses the ESP-IDF SPI slave driver, because the Arduino SPI library can only be a master.

## Rules

| Board | Pins | Does |
|---|---|---|
| Master | button GPIO15; SCK 18, MOSI 23, MISO 19, SS 5 | on each press: send 0x01 or 0x00, print Sent: 0x.. |
| Slave | LED GPIO15 (150 Ω); same SPI pins | LED on at start; 0x01 on, else off; reply 0xAA; print Received: 0x.. |
| Both | 1 MHz, mode 0, 115200 baud |  |

## Parts

- 2 ESP32 DevKits
- 1 push button
- 1 LED
- 1 resistor, 150 Ω
- jumper wires

## Wiring

| Signal | Master | Slave | Direction |
|---|---|---|---|
| SCK | GPIO18 | GPIO18 | Master → Slave |
| MOSI | GPIO23 | GPIO23 | Master → Slave |
| MISO | GPIO19 | GPIO19 | Slave → Master |
| CS | GPIO5 | GPIO5 | Master → Slave |
| GND | GND | GND | common ground |

Connect the GNDs too, or the two boards have no common reference for the signals.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`S2_LED_Master`](S2_LED_Master/S2_LED_Master.ino) | Master ESP32 | 115200 baud |
| [`S2_LED_Slave`](S2_LED_Slave/S2_LED_Slave.ino) | Slave ESP32 | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Button on the master, LED on the slave

| Pin (on its own board) | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO15 | INPUT_PULLUP | master button → GND |
| GPIO15 | output | 150 Ω resistor → slave LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

Both use GPIO15, but on different boards: the button on the master, the LED on the slave.

## One press, step by step

1. Start: slave LED on, master `ledOn = true`
2. Press: master flips to `false` and sends `0x00` on MOSI
3. At the same 8 clocks, the slave sends `0xAA` on MISO (the master ignores it)
4. Slave receives `0x00` → LED off → prints `Received: 0x00`
