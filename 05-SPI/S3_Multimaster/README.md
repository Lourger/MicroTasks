# S3: Multi-Master SPI Communication

Two ESP32 masters share one SD card: one writes a counter, the other reads it.

## The task

Two microcontrollers are both SPI **masters** on the same bus, and one SD card is the shared **slave**. Master 1 has two buttons: one adds 1 to a counter stored on the card, the other resets it to 0. Master 2 has one button that reads the counter and prints it.

SPI has no built-in way for two masters to share a bus, so each master drives the bus only while it uses the card and lets go of every pin afterwards. Both sketches use the same pins, the `SD` library, the file `/count.txt`, and this "release the pins" method.

## Rules

| Board | Buttons | Does |
|---|---|---|
| Master 1 | INC GPIO15, RESET GPIO4 | INC: read, +1, write. RESET: write 0. |
| Master 2 | READ GPIO15 | read and print Stored count is: N |
| Both | SCK 18, MISO 19, MOSI 23, CS 5 | take the bus only when needed, then pins → INPUT (Hi-Z) |
| CS line | 4.7 kΩ pull-up to 3.3 V | keeps the card deselected while both masters let go |

## Parts

- 2 ESP32 DevKits
- microSD card module + card (FAT32)
- 3 push buttons
- 1 resistor, 4.7 kΩ
- jumper wires

## Wiring

| Signal | Master 1 | SD card | Master 2 | Direction |
|---|---|---|---|---|
| SCK | GPIO18 | SCK | GPIO18 | Master 1 → SD card; Master 2 → SD card |
| MOSI | GPIO23 | MOSI | GPIO23 | Master 1 → SD card; Master 2 → SD card |
| MISO | GPIO19 | MISO | GPIO19 | SD card → Master 1; SD card → Master 2 |
| CS | GPIO5 | CS | GPIO5 | Master 1 → SD card; Master 2 → SD card |
| GND | GND | GND | GND | common ground |

Both masters connect to the same four card pins. Power the card module from one board only (3.3 V, or 5 V for a module with a regulator), and connect all GNDs.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`S3_Multimaster_M1`](S3_Multimaster_M1/S3_Multimaster_M1.ino) | Master 1 (counter) | 115200 baud |
| [`S3_Multimaster_M2`](S3_Multimaster_M2/S3_Multimaster_M2.ino) | Master 2 (reader) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Pull-up and buttons

| M1 and M2 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| CS line |  | 4.7 kΩ resistor → 3.3 V |
| GPIO15 | INPUT_PULLUP | M1 INC button → GND |
| GPIO4 | INPUT_PULLUP | M1 RESET button → GND |
| GPIO15 | INPUT_PULLUP | M2 READ button → GND |

## How the masters take turns

| Moment | Master 1 | Master 2 | CS line |
|---|---|---|---|
| idle | pins Hi-Z | pins Hi-Z | HIGH (pull-up) |
| M1 presses INC | checks CS: HIGH, takes the bus, reads and writes | pins Hi-Z | LOW during each card command |
| M1 done | pins Hi-Z | pins Hi-Z | HIGH |
| M2 presses READ | pins Hi-Z | checks CS: HIGH, takes the bus, reads | LOW during each card command |

**The weak spot:** CS is LOW only *during* a card command. Between commands the busy master holds CS HIGH, so if both buttons are pressed at almost the same moment, both masters can take the bus. For this practice task that's acceptable. A sturdier design adds a fourth shared wire, "bus busy", with its own pull-up: a master pulls it LOW for the whole time it owns the bus, and the other master waits while it is LOW.
