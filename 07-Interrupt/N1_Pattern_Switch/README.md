# N1: LED Patterns Switching

A button switches between two LED patterns, using interrupts only, while loop() stays busy.

## The task

Four LEDs show one of two patterns. Each press of a button switches to the other pattern, including quick, repeated presses, and the switch bounce must be filtered. `loop()` is busy with other critical work, so the button can't be handled there: it must use an interrupt. (An interrupt makes the hardware pause `loop()` and run a short function, the ISR or interrupt service routine, then carry on.)

The sketch uses a FALLING interrupt on GPIO13, a 20 ms one-shot hardware timer for the debounce, and a 10 ms hardware timer that steps the pattern (100 ticks = 1 s per frame) and switches immediately after a press.

## Rules

| Rule | Value |
|---|---|
| Button | GPIO13, INPUT_PULLUP, FALLING interrupt |
| LEDs | GPIO15, 16, 17, 18 (150 Ω each) |
| Pattern 0 | 1111 ↔ 0000 |
| Pattern 1 | 1010 ↔ 0101 |
| Frame time | 1 s (100 ticks of 10 ms) |
| Debounce | 20 ms one-shot hardware timer |

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
| GPIO15 | output | 150 Ω resistor → L1 → GND |
| GPIO16 | output | 150 Ω resistor → L2 → GND |
| GPIO17 | output | 150 Ω resistor → L3 → GND |
| GPIO18 | output | 150 Ω resistor → L4 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

The same LED row as GPIO tasks T3 to T5, with the button on GPIO13. Avoid GPIO34 to 39 for buttons: they have no internal pull-up.

## Upload and run

Open `N1_Pattern_Switch/N1_Pattern_Switch.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## The two patterns

| Pattern | Frame time | Frame 1 | Frame 2 |
|---|---|---|---|
| Pattern 0 | 1 s per frame | ● ● ● ● | ○ ○ ○ ○ |
| Pattern 1 | 1 s per frame | ● ○ ● ○ | ○ ● ○ ● |

● = LED on, ○ = LED off. Columns are L1, L2, L3, L4, left to right.

L1 to L4 are the LEDs on GPIO15 to GPIO18. Each pattern alternates between its two frames every second.

## Who does what

| Function | Runs when | Does |
|---|---|---|
| onButtonFalling() | GPIO13 goes HIGH → LOW (an interrupt) | starts the 20 ms debounce timer, unless it's already running |
| onDebounceTimer() | 20 ms later (timer interrupt) | pin still LOW → switch pattern, ask for an instant redraw |
| onPatternTick() | every 10 ms (timer interrupt) | every 100th tick, or right after a switch: next frame |
| loop() | all the rest of the time | busy with its own work (a demo stand-in here); never reads the button |

**What the CPU does:** it runs `loop()` on core 1. When one of these events happens, the hardware pauses `loop()` wherever it is, runs the short function (an ISR, interrupt service routine), and then lets `loop()` continue exactly where it stopped. `loop()` never knows it was paused.

## Debounce, step by step

The first falling edge starts the 20 ms timer. The bounces that follow cause more falling edges, but the ISR ignores them while the timer runs. When the timer fires, the pin is read once: still LOW means a real press.
