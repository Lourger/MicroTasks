# P2: LED Brightness Control Using Potentiometer

Turn a potentiometer to set an LED's brightness smoothly.

## The task

A potentiometer sets the LED's brightness. Turning it from one end to the other moves the brightness smoothly from off to full.

The 12-bit ADC reading (0..4095) is written straight into a 12-bit PWM duty (0..4095): both use the same range, so no conversion is needed.

## Rules

| Rule | Value |
|---|---|
| Input | pot on GPIO34, 12-bit ADC: 0..4095 |
| Output | LED on GPIO14, LEDC 1 kHz, 12-bit duty |
| Conversion | none: reading = duty |
| Update | every 1 ms |

## Parts

- ESP32 DevKit
- 1 LED
- 1 resistor, 150 Ω
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO14 | PWM out | 150 Ω resistor → LED → GND |
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

The pot's two outer pins go to 3.3 V and GND; its middle pin (the wiper) goes to GPIO34, an input-only ADC pin.

## Upload and run

Open `P2_Pot_Brightness/P2_Pot_Brightness.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

This sketch does not use the Serial Monitor.
