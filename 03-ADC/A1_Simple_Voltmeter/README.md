# A1: Simple Voltmeter Using ADC

Read a voltage on GPIO34 and print it in volts every 2 seconds.

## The task

Build a voltmeter: read the voltage on an ADC pin and print it on the Serial Monitor. A potentiometer makes the test voltage.

The ESP32's ADC pins measure from 0 to about 3.3 V, so the pot runs from the 3.3 V pin. Never connect more than 3.3 V to an ESP32 pin.

## Rules

| Rule | Value |
|---|---|
| Input | GPIO34, 12-bit, 11 dB attenuation |
| Formula | V = raw ÷ 4095 × 3.3 |
| Output | Voltage: x.xxx V |
| Every | 2000 ms |

## Parts

- ESP32 DevKit
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires
- a multimeter to compare (optional)

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

Pot ends to 3.3 V and GND, wiper to GPIO34.

## Upload and run

Open `A1_Simple_Voltmeter/A1_Simple_Voltmeter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Worked example: raw 2482

1. `voltage = 2482 ÷ 4095 × 3.3`
2. `= 0.6061 × 3.3 = 2.000 V`
3. Printed: `Voltage: 2.000 V`

## How the ESP32 ADC really behaves

The ESP32 ADC is not perfectly linear. At 11 dB attenuation it has two flat parts: about 0 to 0.14 V reads 0, and above about 3.1 V reads 4095. In between it's close to a straight line, but a few percent off. These are typical values, not a measurement of your chip: compare the printed voltage with a multimeter to see your board's error.
