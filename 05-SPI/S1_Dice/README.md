# S1: SPI Dice Roll

Press a button on the SPI master; the slave answers with a dice roll from 1 to 6.

## The task

Build an SPI **master** with a push button. Each press asks the slave for a dice roll; the slave answers with a random number from 1 to 6, and the master prints it. The slave runs in **SPI mode 2**, so the master must use mode 2 too.

The task is the master (`S1_Dice_Master`). To test it you need a slave that answers with dice rolls: flash `S1_Dice_Slave` on a second ESP32. The master uses the VSPI pins, 4 MHz, mode 2 and a button on GPIO4.

## Rules

| Rule | Value |
|---|---|
| SPI pins | SCK 18, MISO 19, MOSI 23, CS 5 |
| SPI settings | 4 MHz, MSB first, SPI_MODE2 |
| Button | GPIO4, INPUT_PULLUP, 50 ms debounce |
| Each press | one byte exchange: send a dummy byte, receive the roll |
| Output | Dice roll: N (or a not-responding message) |

## Parts

- 2 ESP32 DevKits (master and slave)
- 1 push button
- jumper wires

## Wiring

| Signal | Master | Slave | Direction |
|---|---|---|---|
| SCK | GPIO18 | GPIO18 | Master → Slave |
| MOSI | GPIO23 | GPIO23 | Master → Slave |
| MISO | GPIO19 | GPIO19 | Slave → Master |
| CS | GPIO5 | GPIO5 | Master → Slave |
| GND | GND | GND | common ground |

Same pin numbers on both boards; the Direction column shows which way data flows. Both boards work at 3.3 V, so the four signal lines connect directly, plus a common GND. The master's button is under *The master's button* below.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`S1_Dice_Master`](S1_Dice_Master/S1_Dice_Master.ino) | Master ESP32 (the task: button, prints the rolls) | 115200 baud |
| [`S1_Dice_Slave`](S1_Dice_Slave/S1_Dice_Slave.ino) | Slave ESP32 (answers with rolls) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### S1_Dice_Slave

The slave, for the second board. It answers each request from the master with a random roll from 1 to 6. It uses the ESP-IDF SPI slave driver (part of the ESP32 core, no extra library), explained in more detail in task S2:

- It prepares a random roll before the master asks, because a slave can't choose when the clock starts.
- `spi_slave_transmit()` waits until the master selects the slave and clocks one byte, then the next roll is prepared.
- The internal pull-up on CS keeps it HIGH (not selected) while no master is connected.

## The master's button

| Master ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO4 | INPUT_PULLUP | ROLL button → GND |

## What happens on the wires

Mode 2: the clock rests HIGH. The slave sets each bit before a falling edge; the master reads MISO on every falling edge (8 reads). The bits arrive MSB first: 0000 0101 = 5.

SPI always swaps bytes: while the master sends its dummy `0x00` on MOSI, the slave sends the roll on MISO. That's why `SPI.transfer(0x00)` returns the answer.
