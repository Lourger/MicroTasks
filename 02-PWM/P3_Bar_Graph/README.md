# P3: Bar Graph

Five LEDs fill up one after another as the potentiometer turns.

## The task

Five LEDs act as a bar graph for the potentiometer position. The pot's range is split into **5 equal parts**, one per LED. In the part the pot is in now, that LED's brightness follows the pot. Every LED before it is fully on, and every LED after it is off.

## Rules

| Pot reading | Fading in (dims → bright) | The other LEDs |
|---|---|---|
| 0..818 | LED 1 (GPIO15) | LEDs 2 to 5 off |
| 819..1637 | LED 2 (GPIO16) | LED 1 full; LEDs 3 to 5 off |
| 1638..2456 | LED 3 (GPIO17) | LEDs 1 and 2 full; LEDs 4 and 5 off |
| 2457..3275 | LED 4 (GPIO18) | LEDs 1 to 3 full; LED 5 off |
| 3276..4095 | LED 5 (GPIO19) | LEDs 1 to 4 full |

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
| GPIO15 | output | 150 Ω resistor → LED 1 → GND |
| GPIO16 | output | 150 Ω resistor → LED 2 → GND |
| GPIO17 | output | 150 Ω resistor → LED 3 → GND |
| GPIO18 | output | 150 Ω resistor → LED 4 → GND |
| GPIO19 | output | 150 Ω resistor → LED 5 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `P3_Bar_Graph/P3_Bar_Graph.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.

## Each LED's duty as the pot turns

Each LED rises over its own fifth of the pot range, then stays full.

## Worked example: pot reading 2000

1. Which LED is filling: `2000 ÷ 819 = 2` (integer division) → LED index 2, the third LED
2. Its segment: `2 × 819 = 1638` to `1638 + 819 − 1 = 2456`
3. Its duty: `map(2000, 1638, 2456, 0, 4095) = (2000 − 1638) × 4095 ÷ (2456 − 1638) ≈ 1812` → about 44 %
4. LED 1 and LED 2 are full (4095); LED 4 and LED 5 are off (0)
