# P1: LED Brightness Control Using PWM

Fade one LED from 0 % to 100 % in 2 s, back to 0 % in 1 s, forever.

## The task

Use PWM to change an LED's brightness smoothly. It brightens from 0 % to 100 % over **2 seconds**, then dims back to 0 % over **1 second**, and repeats forever.

The sketch uses the ESP32's LEDC PWM peripheral at 1 kHz with 12-bit duty. (The duty cycle is the share of each period the pin is HIGH; 12 bits give 4096 steps from 0 % to 100 %.)

## Rules

| Rule | Value |
|---|---|
| Rise | 0 % → 100 % in 2 s |
| Fall | 100 % → 0 % in 1 s |
| PWM | LEDC, 1 kHz, 12-bit (0..4095) |
| Steps | every duty value, one at a time (4096 per ramp) |

## Parts

- ESP32 DevKit
- 1 LED
- 1 resistor, 150 Ω
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO14 | PWM out | 150 Ω resistor → LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

Open `P1_LED_PWM_Ramp/P1_LED_PWM_Ramp.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Brightness over time

Up in 2 s, down in 1 s, again and again.

## Worked example: the step time

1. 12-bit duty has `2¹² = 4096` values (0..4095), so one ramp is 4096 steps
2. Rise: `2000 ms ÷ 4096 = 0.488 ms ≈ 488 µs` per step
3. Fall: `1000 ms ÷ 4096 = 0.244 ms ≈ 244 µs` per step
4. Each `ledcWrite()` also takes a few µs, so a real ramp lasts slightly longer than 2 s and 1 s
