# A6: Smart OhmMeter

Measure an unknown resistor, picking the best of four ranges automatically.

## The task

Measure a resistor as accurately as possible over a wide range, and print its value. The unknown resistor forms a voltage divider with one of four known resistors. The program tries all four and uses the one that puts the divider's middle closest to half the supply, where the reading is most accurate.

## Rules

| Range | Drive pin | Known resistor | Good for about |
|---|---|---|---|
| 0 | GPIO25 | 220 Ω | 22 Ω to 2.2 kΩ |
| 1 | GPIO26 | 1.2 kΩ | 120 Ω to 12 kΩ |
| 2 | GPIO27 | 12 kΩ | 1.2 kΩ to 120 kΩ |
| 3 | GPIO32 | 47 kΩ | 4.7 kΩ to 470 kΩ |

## Parts

- ESP32 DevKit
- resistors 220 Ω, 1.2 kΩ, 12 kΩ, 47 kΩ (measure them)
- the resistor to test
- jumper wires
- a multimeter

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO25 |  | 220 Ω resistor → middle |
| GPIO26 |  | 1.2 kΩ resistor → middle |
| GPIO27 |  | 12 kΩ resistor → middle |
| GPIO32 |  | 47 kΩ resistor → middle |
| GPIO34 | ADC | middle → resistor Rx → GND |

All "middle" entries are one point, wired to GPIO34. The unknown resistor Rx goes from that point to GND.

## Upload and run

Open `A6_Ohmmeter/A6_Ohmmeter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Worked example: Rx = 4.7 kΩ

1. Range 2 (12 kΩ): middle = `3300 × 4.7 ÷ (12 + 4.7) ≈ 929 mV`, 721 mV from 1650
2. Range 1 (1.2 kΩ): middle = `3300 × 4.7 ÷ (1.2 + 4.7) ≈ 2629 mV`, 979 mV from 1650
3. Range 3 (47 kΩ): `≈ 300 mV`; range 0 (220 Ω): `≈ 3152 mV`, above the open-circuit limit
4. Closest to 1650 mV: range 2. `Rx = 12000 × 929 ÷ (3300 − 929) ≈ 4702 Ω` → `Resistance: 4.702 kohm`
