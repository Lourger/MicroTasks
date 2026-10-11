# T5: 4-bit Binary Counter

UP and DOWN buttons change a count from 0 to 15, shown in binary on 4 LEDs.

## The task

Two buttons and four LEDs. **UP** adds 1 to the count, **DOWN** subtracts 1, and the four LEDs show the count in binary.

The count stops at the ends: UP at 15 does nothing, and DOWN at 0 does nothing. Each press changes the count exactly once.

## Rules

| Rule | Value |
|---|---|
| Range | 0 to 15 (4 bits) |
| UP | count + 1, stops at 15 |
| DOWN | count − 1, stops at 0 |
| Debounce | 50 ms, one change per press |
| LED order | bit 0 = GPIO15, bit 1 = GPIO16, bit 2 = GPIO17, bit 3 = GPIO18 |

## Parts

- ESP32 DevKit
- 2 push buttons
- 4 LEDs
- 4 resistors, 150 Ω
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO13 | INPUT_PULLUP | UP button → GND |
| GPIO14 | INPUT_PULLUP | DOWN button → GND |
| GPIO15 | output | 150 Ω resistor → LED bit 0 → GND |
| GPIO16 | output | 150 Ω resistor → LED bit 1 → GND |
| GPIO17 | output | 150 Ω resistor → LED bit 2 → GND |
| GPIO18 | output | 150 Ω resistor → LED bit 3 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `T5_Binary_Counter/T5_Binary_Counter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.

## Count to LEDs

| Count | Binary | LEDs b3 b2 b1 b0 |
|---|---|---|
| 1 | 0001 | ○ ○ ○ ● |
| 6 | 0110 | ○ ● ● ○ |
| 11 | 1011 | ● ○ ● ● |
| 15 | 1111 | ● ● ● ● |

● = LED on, ○ = LED off. Columns are b3, b2, b1, b0, left to right.

Read the LEDs from bit 3 (left) to bit 0 (right), the same way you write a binary number.

## Worked example: stopping at the ends

1. UP at 15: `count < 15` → `15 < 15` is false, so nothing happens
2. DOWN at 0: `count > 0` → `0 > 0` is false, so nothing happens
3. Why the DOWN check matters: `count` is a `uint8_t` (0 to 255). Without the check, `0 − 1` stored in it becomes `255 = 0b1111_1111`
4. The LEDs would then show `1111` (15) while the variable really holds 255, and the next UP would go to 0
