# N3: Single, Double and Long Press with Interrupts

Single press toggles LED1, double press toggles LED2, long press turns both off: all with interrupts.

## The task

One button, two LEDs. A **single** press toggles LED1, a **double** press toggles LED2, and a **long** press (more than 1 s) turns both off. Bounces must be filtered, and `loop()` is busy, so everything runs in interrupts.

The sketch uses a FALLING interrupt on GPIO27, a 50 ms one-shot timer for the debounce, and a 500 ms timer that judges the clicks 500 ms and 1000 ms after the first press. Compare with GPIO task T2 (Single, Double Click and Long Press), where `loop()` polls the button.

## Rules

| Press | Detected when | Action |
|---|---|---|
| single | 1 click, released by the 500 ms tick | toggle LED1 (GPIO5) |
| double | 2 or more clicks by the 500 ms tick | toggle LED2 (GPIO18) |
| long | 1 click, still held at the 1000 ms tick | both LEDs off |
| debounce | 50 ms after each falling edge | pin still LOW = a real click |

## Parts

- ESP32 DevKit
- 2 LEDs
- 2 resistors, 150 Ω
- 1 push button
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO27 | INPUT_PULLUP | button → GND |
| GPIO5 | output | 150 Ω resistor → LED1 → GND |
| GPIO18 | output | 150 Ω resistor → LED2 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `N3_Click_Detect/N3_Click_Detect.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.

## When each case is decided

The first press starts the 500 ms window timer. The timer ISR decides at the first tick (500 ms after the first press) or, if the button is still held, at the second tick (1000 ms).

One more case: a press released between the 500 ms and 1000 ms ticks is neither a single nor a long press, so nothing happens.

## Two timers, two jobs

| Timer | Alarm | Started by | Stopped by |
|---|---|---|---|
| debounce | 50 ms | each press | its own ISR |
| window | every 500 ms | first press | its own ISR, once decided |

Each timer remembers whether it is running (`debounceRunning`, `windowRunning`), so a press during a running debounce check, or during an open window, doesn't start it twice.
