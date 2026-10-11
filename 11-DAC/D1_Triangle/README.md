# D1: Triangular Wave Generator

Output a triangle wave on the DAC, with its amplitude and frequency set by two pots.

## The task

Generate a triangle wave whose amplitude is set by one potentiometer and whose frequency is set by another, over as wide a frequency range as you can.

The sketch uses DAC1 on GPIO25, the amplitude pot on GPIO34 (1 to 255 DAC codes), the frequency pot on GPIO35 (1 to 500 Hz), both pots averaged over 5 readings every 100 ms, and a `micros()` check that outputs one step at a time, so `loop()` never waits. Nothing is printed; you watch the output on an oscilloscope. When the amplitude is turned down, the value being output is brought inside the new peak at once.

## Rules

| Rule | Value |
|---|---|
| Output | GPIO25 (DAC1), from 0 up to the amplitude |
| Amplitude | pot on GPIO34 → 1 .. 255 (13 mV .. 3.3 V peak) |
| Frequency | pot on GPIO35 → 1 .. 500 Hz |
| Pots | average of 5 readings, every 100 ms |
| Step time | 1,000,000 ÷ (2 × amplitude × frequency) µs |

## Parts

- ESP32 DevKit
- 2 potentiometers (10 kΩ)
- an oscilloscope (or a multimeter, see below)
- optional RC filter: 1 kΩ resistor + 100 nF capacitor
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | amplitude pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO35 | ADC input | frequency pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO25 | DAC1 out | scope CH1 |

Each pot: outer pins to 3.3 V and GND, middle pin to the GPIO. Scope ground to the ESP32's GND. For a smooth line, put the RC filter between GPIO25 and the scope (below).

## Upload and run

Open `D1_Triangle/D1_Triangle.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: not needed. The sketch opens the serial port at 115200 baud but prints nothing.

## One period, step by step

One period is 2 × amplitude steps: up from 0 to the amplitude, then back down. At amplitude 128 there are 256 steps per period. At the same frequency, half the amplitude means half the steps, so each step lasts twice as long. That's what the step-time formula does.

## Worked example: the step time

1. Amplitude 128 at 250 Hz: one period is `2 × 128 = 256` steps and lasts `1 ÷ 250 = 4,000 µs`
2. Step time `4,000 ÷ 256 = 15.6 µs`; integer division keeps `15 µs` → period `256 × 15 = 3,840 µs` → `260 Hz`, 4 % high
3. At low frequencies the cut-off fraction hardly matters: amplitude 128 at 10 Hz → `390.6 → 390 µs`, 0.2 % off
4. Amplitude 255 at 500 Hz: `1,000,000 ÷ 255,000 = 3.9 → 3 µs`. Now two errors pull against each other: the cut-off makes it faster, while the time each `loop()` pass takes (and `lastStepUs = now`, which lets a late step push the next one later) makes it slower. Measure the top end on the scope

## Smoothing the steps: an RC filter

The DAC holds each value until the next one, so the output is a staircase (zoom in on the scope to see it). A resistor and a capacitor make a **low-pass filter** that rounds the steps off: 1 kΩ from GPIO25 to the scope probe, 100 nF from the probe point to GND.

1. Cut-off frequency `fc = 1 ÷ (2π × R × C) = 1 ÷ (2π × 1,000 × 0.0000001) ≈ 1.6 kHz`
2. Pick fc well above the wave's frequency (it passes) and well below the step rate (the steps don't): amplitude 128 at 250 Hz steps at about 64 kHz
3. Near 500 Hz the filter also rounds the triangle's sharp corners a little, because the corners are made of higher frequencies

The DAC pin can't drive much current. A scope or another chip's input is fine; for a speaker or any low-resistance load, add an op-amp buffer.

## No oscilloscope?

Turn the frequency pot all the way down (1 Hz) and measure GPIO25 with a multimeter on DC volts: the reading climbs and falls once a second. With the amplitude at maximum it swings between 0 V and about 3.3 V (in practice a little less at the top).
