# T4: LED Blinking Patterns

Four LEDs, five patterns, one button: each press switches to the next pattern at once.

## The task

Connect four LEDs and one push button. The LEDs play one of **5 patterns**, one step every 500 ms. Each press switches straight to the next pattern, starting from its first step, and after the last pattern it goes back to the first.

The 5 patterns are numbered 0 to 4, as in the code.

## Rules

| # | Steps (LED1..LED4) | Step time |
|---|---|---|
| 0 | all ON, then all OFF | 500 ms |
| 1 | LED1 + LED3, then LED2 + LED4 | 500 ms |
| 2 | LED1, LED2, LED3, LED4, one at a time | 500 ms |
| 3 | LED1 + LED2, then LED3 + LED4 | 500 ms |
| 4 | LED1 + LED4, then LED2 + LED3 | 500 ms |

## Parts

- ESP32 DevKit
- 4 LEDs
- 4 resistors, 150 Ω
- 1 push button
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO13 | INPUT_PULLUP | button → GND |
| GPIO15 | output | 150 Ω resistor → LED1 → GND |
| GPIO16 | output | 150 Ω resistor → LED2 → GND |
| GPIO17 | output | 150 Ω resistor → LED3 → GND |
| GPIO18 | output | 150 Ω resistor → LED4 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `T4_Blink_Patterns/T4_Blink_Patterns.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.

## The five patterns, step by step

| Pattern | Step time | Step 1 | Step 2 | Step 3 | Step 4 |
|---|---|---|---|---|---|
| 0 All on/off | 500 ms | ● ● ● ● | ○ ○ ○ ○ |  |  |
| 1 Odd / even | 500 ms | ● ○ ● ○ | ○ ● ○ ● |  |  |
| 2 Running | 500 ms | ● ○ ○ ○ | ○ ● ○ ○ | ○ ○ ● ○ | ○ ○ ○ ● |
| 3 Halves | 500 ms | ● ● ○ ○ | ○ ○ ● ● |  |  |
| 4 Outer / inner | 500 ms | ● ○ ○ ● | ○ ● ● ○ |  |  |

● = LED on, ○ = LED off. Columns are L1, L2, L3, L4, left to right.

L1 to L4 are LED1 to LED4 (GPIO15 to GPIO18). After the last step, the pattern starts again from its first step.
