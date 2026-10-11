# T1: Simultaneous Blinking LEDs

Blink two LEDs at exact, unrelated rates (100 ms and 360 ms) while loop() stays free.

## The task

Two LEDs blink at the same time: LED1 toggles every 100 ms and LED2 every 360 ms, as precisely as possible. The main loop is busy with critical work, so the blinking must not depend on it.

The sketch uses one hardware timer per LED, both counting at 1 MHz, with auto-reloading alarms at 100,000 µs and 360,000 µs; each timer's ISR toggles its LED, and `loop()` is empty.

## Rules

| LED | Pin | Toggles every | Full period |
|---|---|---|---|
| LED1 | GPIO2 (150 Ω) | 100 ms | 200 ms |
| LED2 | GPIO4 (150 Ω) | 360 ms | 720 ms |

## Parts

- ESP32 DevKit
- 2 LEDs
- 2 resistors, 150 Ω
- jumper wires

## Wiring

| ESP32 pin | Connection (from the pin outward) |
|---|---|
| GPIO2 | 150 Ω resistor → LED1 → GND |
| GPIO4 | 150 Ω resistor → LED2 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

On many DevKits, GPIO2 also drives the small on-board LED, which then blinks with LED1.

## Upload and run

Open `T1_Dual_Blink/T1_Dual_Blink.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.

## The two blink patterns

Each LED has its own timer, so neither waits for the other. Both start off; they are back in the same state together every 3600 ms (the smallest time that is a whole number of both periods, 200 ms and 720 ms).

## Why timers instead of millis()

A `millis()` check in `loop()` only works while `loop()` runs often; here it is busy with other work, so the LEDs would toggle late. A hardware timer counts by itself and interrupts the CPU exactly when its alarm value is reached, whatever `loop()` is doing.
