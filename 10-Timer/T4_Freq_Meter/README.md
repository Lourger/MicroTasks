# T4: High-Frequency Measurement

Measure square-wave frequencies above 100 kHz (up to 8 MHz) and print them in Hz.

## The task

Measure the frequency of a square wave above 100 kHz, over the widest range you can. To test it, you make the signal yourself: a second ESP32 running `T4_Signal_Generator` outputs a square wave from 100 kHz to 8 MHz, set with a potentiometer.

The meter's PCNT peripheral counts rising edges on GPIO4 for a 10 ms window, the count is turned into hertz, and one result prints per second as `Frequency: … Hz`. Two details make it accurate, both explained below: the window is timed exactly, and counter overflows are added up, so the range reaches 8 MHz.

## Rules

| Rule | Value |
|---|---|
| Input | GPIO4, rising edges |
| Window | about 10 ms, measured with esp_timer |
| Output | Frequency: 1000000.00 Hz, every ~1 s |
| Range | 100 kHz to 8 MHz (and beyond) |

## Parts

- 2 ESP32 DevKits (meter + signal generator), or one with an LEDC test signal
- 1 potentiometer (for the generator)
- jumper wires

## Wiring

| Signal | Generator | Meter | Direction |
|---|---|---|---|
| signal | GPIO5 | GPIO4 | Generator → Meter |
| GND | GND | GND | common ground |

The generator's pot goes on its GPIO34. Keep the signal wire short.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`T4_Freq_Meter`](T4_Freq_Meter/T4_Freq_Meter.ino) | Meter ESP32 (the task) | 115200 baud |
| [`T4_Signal_Generator`](T4_Signal_Generator/T4_Signal_Generator.ino) | Generator ESP32 (100 kHz to 8 MHz) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### T4_Signal_Generator

For the second board: a pot sets an LEDC square wave from 100 kHz to 8 MHz, on an exponential scale (each part of the pot's turn multiplies the frequency by the same factor). It prints the frequency LEDC really produced, so you can compare it with the meter. One board only? Add `ledcAttach(5, 1000000, 1); ledcWrite(5, 1);` to the meter's `setup()` and wire GPIO5 to GPIO4.

## Two details, worked out

1. **Window length.** `delay(10)` waits for 10 FreeRTOS ticks of 1 ms, but the first tick can come at any moment, so the real window is anywhere from 9 to 10 ms
2. Simply multiplying the count by 100 (as if the window were exactly 10 ms) goes wrong: a 1 MHz signal counted for 9.43 ms gives 9,430 counts → 943,000 Hz, 5.7 % low
3. The sketch times the window with `esp_timer_get_time()` (1 µs steps): `9,430 × 1,000,000 ÷ 9,430 µs = 1,000,000 Hz`
4. **Range.** At 8 MHz, 10 ms holds 80,000 edges, more than the 16-bit counter's 32,767. Without overflow handling, a 10 ms window tops out near 3.2 MHz; the sketch sets the driver's `accum_count` option, which adds up the overflows

## Resolution

Counting for 10 ms means each edge counted is worth 100 Hz, so a result is only ever accurate to about ±100 Hz, however many decimals are printed. A longer window gives finer results (1 s → 1 Hz) but slower updates; for low frequencies, timing one period is better than counting edges.
