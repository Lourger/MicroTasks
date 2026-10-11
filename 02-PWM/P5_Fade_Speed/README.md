# P5: LEDs Fading Speed Control

Five LEDs fade in and out in turn; a potentiometer sets how fast.

## The task

Five LEDs fade in two alternating groups: while LEDs 1, 3 and 5 fade in, LEDs 2 and 4 fade out, then the other way round. A potentiometer sets the speed: at 0 % the fading is fast, at 100 % it is slow.

## Rules

| Rule | Value |
|---|---|
| Group A | GPIO15, 17, 19 |
| Group B | GPIO16, 18 (always the opposite of A) |
| Step delay | pot 0 % → `10 µs`, pot 100 % → `1000 µs`, per duty step |
| Steps | duty 0..4095 one at a time, then back |
| Pause at each end | 1500 × the step delay (15 ms to 1.5 s) |
| Pot read | once per full cycle; the step delay is printed on the Serial Monitor |

## Parts

- ESP32 DevKit
- 5 LEDs
- 5 resistors, 150 Ω
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO15 | output | 150 Ω resistor → LED A → GND |
| GPIO16 | output | 150 Ω resistor → LED B → GND |
| GPIO17 | output | 150 Ω resistor → LED A → GND |
| GPIO18 | output | 150 Ω resistor → LED B → GND |
| GPIO19 | output | 150 Ω resistor → LED A → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `P5_Fade_Speed/P5_Fade_Speed.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Fade time vs pot position

One fade = 4096 steps × step delay: 4096 × 10 µs ≈ 0.04 s up to 4096 × 1000 µs ≈ 4.1 s.
