# T3: Logic Gate Implementation

Two switches in, four LEDs out: show OR, AND, NOR and NAND at the same time.

## The task

Two switches are the inputs A and B. Four LEDs are the outputs. Each LED shows the result of one logic gate using A and B: **OR**, **AND**, **NOR** and **NAND**.

The pin level *is* the input bit: an open switch reads HIGH = 1 (held up by the pull-up), and a switch closed to GND reads LOW = 0.

## Rules

| A | B | OR | AND | NOR | NAND |
|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 1 | 1 |
| 0 | 1 | 1 | 0 | 0 | 1 |
| 1 | 0 | 1 | 0 | 0 | 1 |
| 1 | 1 | 1 | 1 | 0 | 0 |

## Parts

- ESP32 DevKit
- 2 slide or toggle switches
- 4 LEDs
- 4 resistors, 150 Ω
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO14 | INPUT_PULLUP | switch A → GND |
| GPIO13 | INPUT_PULLUP | switch B → GND |
| GPIO15 | output | 150 Ω resistor → LED OR → GND |
| GPIO16 | output | 150 Ω resistor → LED AND → GND |
| GPIO17 | output | 150 Ω resistor → LED NOR → GND |
| GPIO18 | output | 150 Ω resistor → LED NAND → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

With a 3-pin slide switch: middle pin to the GPIO, one side pin to GND, the other side pin unused. Closed to GND reads 0; open reads 1. Each LED has its own resistor.

## Upload and run

Open `T3_Logic_Gates/T3_Logic_Gates.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.
