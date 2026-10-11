# T3: High-Frequency Pulses Counter

Count incoming pulses (up to 8 MHz) in hardware; a button prints and resets the count.

## The task

Count the pulses arriving on a pin, at up to 8 MHz. Each press of a button prints the count so far and resets it to 0. Any method may generate the test pulses. Here you make them yourself: a second ESP32 running `T3_Burst_Generator` sends exactly 1000 short pulses per press of its button.

The counter uses the ESP32's **PCNT** (pulse counter) peripheral on GPIO4, counting rising edges, with an interrupt that counts each time the 16-bit hardware counter overflows; a 50 ms debounced button on GPIO14; `Number of Pulses: N` on each press.

## Rules

| Rule | Value |
|---|---|
| Input | GPIO4, rising edges, up to 8 MHz |
| Counter | PCNT, 16-bit, overflow at 32767 counted in software |
| Button | GPIO14, INPUT_PULLUP, 50 ms debounce |
| Output | Number of Pulses: N (then reset to 0) |

## Parts

- 2 ESP32 DevKits (counter + burst generator), or one with an LEDC test signal
- 2 push buttons
- jumper wires

## Wiring

| Signal | Generator | Counter | Direction |
|---|---|---|---|
| pulses | GPIO4 | GPIO4 | Generator → Counter |
| GND | GND | GND | common ground |

Plus a button to GND on each board: GPIO15 on the generator, GPIO14 on the counter. Keep the pulse wire short: at MHz speeds a long loose wire rings.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`T3_Pulse_Counter`](T3_Pulse_Counter/T3_Pulse_Counter.ino) | Counter ESP32 (the task) | 115200 baud |
| [`T3_Burst_Generator`](T3_Burst_Generator/T3_Burst_Generator.ino) | Generator ESP32 (1000 pulses per press) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### T3_Burst_Generator

For the second board: each button press sends exactly 1000 pulses, so the counter should print `Number of Pulses: 1000` after one burst and 2000 after two. It toggles the pin by writing the GPIO registers directly with a few `nop` (do-nothing) instructions between, which gives pulses of roughly 0.1 µs at a few MHz; a logic analyser shows the exact rate. One board only? Add `ledcAttach(5, 1000000, 1); ledcWrite(5, 1);` to the counter's `setup()` and wire GPIO5 to GPIO4: a 1 MHz test signal, so the count is about 1,000,000 per second between presses.

## How the total is built

1. The hardware counter holds 0 to 32767. When a pulse brings it to 32767, the hardware clears it to 0 and the interrupt adds 1 to `overflowCount`
2. So each overflow stands for 32767 pulses. Example: 70,000 pulses → 2 overflows (65,534) + counter 4,466
3. Total = `2 × 32767 + 4466 = 70,000`
4. Adding 32768 per overflow would count one pulse too many each time; 32767 is also what ESP-IDF's own overflow option adds

## Driver version

The sketch uses the current PCNT driver (`driver/pulse_cnt.h`), not the older `driver/pcnt.h`, which ESP-IDF 5 marks as deprecated (it prints a warning when you build). Settings: rising edges count, falling edges are ignored, no control pin, no glitch filter.
