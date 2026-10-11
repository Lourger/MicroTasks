# A2: Precision Voltmeter Using ADC

A voltmeter that uses a fine range for small voltages and a coarse range for large ones.

## The task

Measure 0 to about 3.1 V, with finer steps for small voltages. Below about 0.85 V, the ADC uses its **fine range** (0 dB); above, its **coarse range** (11 dB). The program switches between them by itself.

## Rules

| Range | Attenuation | About | Step size |
|---|---|---|---|
| fine | ADC_0db | 0 to 0.95 V | 950 mV ÷ 4095 ≈ 0.23 mV |
| coarse | ADC_11db | 0 to 3.1 V | 3100 mV ÷ 4095 ≈ 0.76 mV |

## Parts

- ESP32 DevKit
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

## Upload and run

Open `A2_Precision_Voltmeter/A2_Precision_Voltmeter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## When it switches

| Now in | First reading | Then |
|---|---|---|
| fine | raw = 4095 (saturated) | switch to coarse |
| coarse | below 850 mV | switch to fine |
| either | anything else | stay |

850 mV is a little under the fine range's 950 mV top, so a voltage near the edge doesn't keep flipping between the two.
