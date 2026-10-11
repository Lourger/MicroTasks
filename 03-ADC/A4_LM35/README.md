# A4: Interface LM35 Temperature Sensor

Read an LM35 and print the temperature, including below 0 °C.

## The task

Connect an LM35 temperature sensor and print the temperature on the Serial Monitor. The design must also handle **negative temperatures**.

The LM35's GND pin is lifted to about 1.24 V by a resistor divider, so its output can sit below that pin for negative temperatures. A second ADC pin measures the lifted GND, and the code subtracts it.

## Rules

| Rule | Value |
|---|---|
| Sensor | LM35: 10 mV per °C, powered from 5 V |
| Pins | Vout → GPIO34, LM35 GND pin → GPIO35 |
| Formula | °C = (Vout mV − GND mV) ÷ 10 |
| Output | Temperature: xx.xx C, every 1 s |

## Parts

- ESP32 DevKit
- LM35
- resistors 10 kΩ and 3.3 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| 5V |  | LM35 +Vs |
| GPIO34 | ADC | LM35 Vout |
| GPIO35 | ADC | LM35 GND → 3.3 kΩ resistor → GND |
| 5V |  | 10 kΩ resistor → LM35 GND |

Both "LM35 GND" entries are the same point: the LM35's GND pin, the middle of the divider, and GPIO35. Use the DevKit's 5V (VIN) pin for 5 V.

## Upload and run

Open `A4_LM35/A4_LM35.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Worked example: the lifted GND

1. Divider: `5 V × 3.3 kΩ ÷ (10 kΩ + 3.3 kΩ) = 1.24 V` on the LM35 GND pin
2. At 25 °C the LM35 sits `25 × 10 mV = 250 mV` above its own GND pin: Vout ≈ `1.49 V`
3. At −10 °C it sits 100 mV *below* it: Vout ≈ `1.14 V`, still a positive voltage the ADC can read
4. Code: `(1490 − 1240) ÷ 10 = 25.0 °C` and `(1140 − 1240) ÷ 10 = −10.0 °C`

For readings below 0 °C, the LM35's output must be able to go lower than its own GND pin. The LM35 datasheet's single-supply circuit for this also adds a pull-down resistor (18 kΩ) from Vout to real GND. Add that resistor if your readings stop at 0 °C.
