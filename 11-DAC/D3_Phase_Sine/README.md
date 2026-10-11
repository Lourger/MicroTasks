# D3: Phase-shifted Sine Wave

Two sine waves on the two DACs; a pot shifts the second one by 0° to 360°.

## The task

Generate two sine waves: the first is a plain sine wave, the second is the same wave shifted in phase by an amount set with a potentiometer, anywhere from 0° to 360°.

The sketch uses one 256-entry sine table, DAC1 (GPIO25) and DAC2 (GPIO26), and the pot on GPIO34, read once per period and turned into degrees (÷ 11.375) and then into a table offset. It waits 100 µs after each pair of samples.

## Rules

| Rule | Value |
|---|---|
| Wave 1 | GPIO25 (DAC1): sineTable[i] |
| Wave 2 | GPIO26 (DAC2): sineTable[(i + offset) % 256] |
| Pot | GPIO34 → 0 .. 360° → offset 0 .. 256 samples |
| Timing | 100 µs per sample, 256 samples: about 39 Hz |

## Parts

- ESP32 DevKit
- 1 potentiometer (10 kΩ)
- a two-channel oscilloscope
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | phase pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO25 | DAC1 out | scope CH1 |
| GPIO26 | DAC2 out | scope CH2 |

Both scope grounds to the ESP32's GND.

## Upload and run

Open `D3_Phase_Sine/D3_Phase_Sine.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: not needed. The sketch opens the serial port at 115200 baud but prints nothing.

## The two outputs at 90°

Pot at a quarter turn: offset 64 samples = 90°. DAC2 reads 64 entries further on in the table, so it reaches each point of the wave earlier: it leads DAC1 by 90°.

## Worked example: pot to offset

1. Pot at a quarter turn, reading 1024: `1024 ÷ 11.375 = 90.0°` (11.375 = 4095 ÷ 360)
2. Offset = `90 ÷ 360 × 256 = 64` samples
3. One sample is `360 ÷ 256 ≈ 1.4°`, so the shift moves in 1.4° steps
4. Pot at 4095: 360° → offset 256, and `(i + 256) % 256 = i`: the same as 0°, as it should be

## How fast is it really?

1. Ideal: `256 × 100 µs = 25.6 ms` per period → `39 Hz`
2. Each sample also costs two `dacWrite()` calls and a loop step, a few microseconds that add up over 256 samples, so a scope shows a little under 39 Hz
3. For a higher frequency: a smaller `SAMPLE_US`, or fewer samples per period

## X-Y mode: the phase at a glance

Most scopes have an X-Y mode: CH1 moves the dot left and right, CH2 up and down. With the two DACs on CH1 and CH2, the shape shows the phase shift directly (a **Lissajous figure**).

Same frequency on both channels: 0° draws a rising line, 90° a circle, 180° a falling line, anything between an ellipse.
