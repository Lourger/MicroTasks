# T2: Simultaneous Square Wave Generation

Output a 307 kHz and a 570 kHz square wave at the same time, within ±1 %.

## The task

Generate two square waves at the same time: 307 kHz on one pin and 570 kHz on another, each within ±1 % of its target. The main loop is busy, so the waves must come from hardware.

The main sketch uses the LEDC peripheral, set up with the ESP-IDF `driver/ledc.h` functions, two LEDC timers (one per frequency) in high-speed mode, 2-bit resolution, duty 2 of 4 (50 %), on GPIO4 and GPIO5.

## Rules

| Wave | Pin | Frequency | LEDC timer / channel |
|---|---|---|---|
| 1 | GPIO4 | 307 kHz | timer 0 / channel 0 |
| 2 | GPIO5 | 570 kHz | timer 1 / channel 1 |
| both |  | ±1 %, 50 % duty | 2-bit, high-speed mode |

## Parts

- ESP32 DevKit
- an oscilloscope or logic analyser to check (or the frequency meter from task T4 in this section)

## Upload and run

This task has more than one version of the program. Each runs on its own on the same board and wiring; flash the one you want.

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`T2_Square_Waves`](T2_Square_Waves/T2_Square_Waves.ino) | ESP-IDF LEDC driver version | not used |
| [`T2_Square_Waves_Arduino`](T2_Square_Waves_Arduino/T2_Square_Waves_Arduino.ino) | Arduino LEDC version | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### T2_Square_Waves_Arduino: Arduino LEDC version

The same two waves with the Arduino core's LEDC functions, which do the timer and channel setup for you. It also prints the frequencies LEDC actually set.

## What the outputs look like

Periods: 1 ÷ 307 kHz ≈ 3.26 µs and 1 ÷ 570 kHz ≈ 1.75 µs. Both run from the LEDC hardware, so the CPU isn't involved at all.

## Worked example: is it within ±1 %?

1. The LEDC timer divides the 80 MHz clock, then counts 4 steps (2 bits) per period: divider = `80,000,000 ÷ (307,000 × 4) = 65.15`
2. The divider has 8 fractional bits, so it becomes `65 + 38/256 = 65.148` → `80 MHz ÷ (4 × 65.148) ≈ 306,993 Hz`: 0.002 % low
3. 570 kHz: divider `35.088 → 35.086` → `≈ 570,028 Hz`: 0.005 % high
4. Both well inside ±1 %. The fraction is made by alternating whole dividers, so single periods differ by about one 12.5 ns clock step; the average is exact

## Why only 2 bits?

LEDC's rule: frequency × 2^(bits) must not exceed its 80 MHz clock. At 570 kHz, 2^(bits) can be at most 140, so up to 7 bits would fit; but a square wave only needs one duty value (50 %), and fewer bits leave a larger, finer divider. With 2 bits, 50 % is a duty of 2 out of 4.
