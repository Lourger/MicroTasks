# N2: Short Pulse Detection

Catch pulses about 0.1 µs long and toggle an LED on each one, without touching the busy loop().

## The task

A pulse generator sends very short pulses, about 100 ns (0.1 µs) or less. Toggle an LED on every pulse, immediately, without missing any. `loop()` holds a fixed busy `while (true)` block that must not be changed.

The pulse arrives on GPIO4, a RISING interrupt catches it, and the ISR toggles the LED on GPIO2 directly. You provide the pulses yourself with a second ESP32 running `N2_Pulse_Generator`: it sends one very short pulse on its GPIO4 every 5 seconds and prints `Pulse generated` each time, so every message on its Serial Monitor should match one LED toggle on the detector.

## Rules

| Rule | Value |
|---|---|
| Input | GPIO4, RISING interrupt |
| Output | LED on GPIO2 (150 Ω), toggles on every pulse |
| Pulse | about 100 ns or shorter, one every 5 s (from `N2_Pulse_Generator`) |
| loop() | while (true) { } : must not change |

## Parts

- 2 ESP32 DevKits (detector and pulse generator)
- 1 LED
- 1 resistor, 150 Ω
- jumper wires

## Wiring

| Signal | Generator | Detector | Direction |
|---|---|---|---|
| pulse | GPIO4 | GPIO4 | Generator → Detector |
| GND | GND | GND | common ground |

Generator GPIO4 to detector GPIO4, plus a common GND. Both boards work at 3.3 V, so they connect directly. Keep the wire short, so the very short pulse keeps its sharp edges.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`N2_Pulse_Detect`](N2_Pulse_Detect/N2_Pulse_Detect.ino) | Detector ESP32 (the task: LED) | not used |
| [`N2_Pulse_Generator`](N2_Pulse_Generator/N2_Pulse_Generator.ino) | Generator ESP32 (sends the pulses) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### N2_Pulse_Generator

The pulse source, for the second board: one pulse every 5 seconds on GPIO4. To make the pulse as short as possible it skips `digitalWrite()` and writes the GPIO registers directly: one write sets the pin HIGH, the next sets it LOW. `noInterrupts()` makes sure nothing runs between the two writes. The pulse lasts roughly 0.1 µs (a logic analyser or scope shows the exact width).

- `GPIO_OUT_W1TS_REG`: write a 1 bit to set that pin HIGH (W1TS = "write 1 to set").
- `GPIO_OUT_W1TC_REG`: write a 1 bit to set that pin LOW ("write 1 to clear").

## The detector's LED

| Detector pin | Connection (from the pin outward) |
|---|---|
| GPIO2 | 150 Ω resistor → LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Why polling can't catch it

The GPIO hardware notices the rising edge at once and remembers it, even though the pulse is already over. The ISR starts a little later (a few microseconds with attachInterrupt) and toggles the LED. A loop that reads the pin would have to read during the 0.1 µs the pulse lasts.

Code that checks the pin with `digitalRead()` only sees the level at the moment it reads. The pulse is over in 0.1 µs, far shorter than one pass of any loop, so a poll almost always looks at the wrong moment. The interrupt doesn't depend on timing: the edge is caught by hardware.
