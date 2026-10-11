# A5: Sine Wave Analyzer

Generate a sine wave with the DAC, read it back with the ADC, and print peak, RMS and average.

## The task

Feed a sine wave into the ADC and work out three values: its **peak**, its **RMS** and its **average**. The ESP32 makes the sine wave itself with its built-in DAC (digital-to-analog converter), and a jumper wire feeds it back into an ADC pin. The wave stays inside the ADC's most accurate range (about 0.8 to 2.5 V), and the sketch reads calibrated millivolts with `analogReadMilliVolts()`.

## Rules

| Rule | Value |
|---|---|
| Sine | DAC GPIO25, 360 points, 128 ± 64 counts |
| Sampling | GPIO34, calibrated mV, for 1 s |
| Peak | (max − min) ÷ 2 |
| RMS | peak ÷ √2 |
| Average | 0.637 × peak |
| Output | Peak: … RMS: … Average: … (3 decimals, tab-separated) |

## Parts

- ESP32 DevKit
- 1 jumper wire

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO25 | DAC1 out | jumper wire → GPIO34 |

One wire from GPIO25 (DAC1) to GPIO34 (ADC). Nothing else is needed.

## Upload and run

Open `A5_Sine_Analyzer/A5_Sine_Analyzer.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Worked example: the expected values

1. DAC step: `3.3 V ÷ 255 ≈ 12.9 mV`; offset 128 → about `1.65 V`
2. Amplitude 64 counts → peak `64 × 12.9 mV ≈ 0.83 V`, so the wave swings from about 0.82 V to 2.48 V
3. RMS: `0.83 ÷ 1.414 ≈ 0.59 V`
4. Average: `0.637 × 0.83 ≈ 0.53 V` (the average of the wave with its negative half flipped up; the plain average of a full sine is 0)
