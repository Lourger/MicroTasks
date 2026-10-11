# D2: Multi-Waveform Generator

Sine, square or triangle on the DAC: pots set amplitude and frequency, a button changes the shape.

## The task

Generate a sine, square or triangle wave. One potentiometer sets the amplitude, another the frequency, and a push button cycles Sine → Square → Triangle → Sine. It starts with a sine wave, and pot changes show on the output right away.

The sketch uses DAC1 on GPIO25, the amplitude pot on GPIO34 (1 to 255), the frequency pot on GPIO35 on an exponential scale from 10 Hz to 1000 Hz, the button on GPIO32 with the internal pull-up and a 200 ms lock-out, a 256-entry sine table and 256 samples per period, timed with `micros()`. Three design details are explained below: the order of the shapes, a triangle that runs at the set frequency, and the printed waveform name.

## Rules

| Rule | Value |
|---|---|
| Output | GPIO25 (DAC1) |
| Button | GPIO32 to GND, INPUT_PULLUP, 200 ms lock-out |
| Order | SINE → SQUARE → TRIANGLE → SINE (starts as SINE) |
| Amplitude | pot on GPIO34 → 1 .. 255 |
| Frequency | pot on GPIO35 → 10 .. 1000 Hz, exponential |
| Samples | 256 per period: step = 1,000,000 ÷ (256 × f) µs |
| Message | Waveform: SQUARE (on each change) |

## Parts

- ESP32 DevKit
- 2 potentiometers (10 kΩ)
- 1 push button
- an oscilloscope
- optional RC filter (see task D1)
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | amplitude pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO35 | ADC input | frequency pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO32 | INPUT_PULLUP | wave button → GND |
| GPIO25 | DAC1 out | scope CH1 |

The button is on GPIO32, because GPIO25 is the DAC output.

## Upload and run

Open `D2_Multi_Wave/D2_Multi_Wave.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## The three shapes

Each shape is 256 samples per period; the amplitude pot scales every sample by amplitude ÷ 255.

## Why an exponential frequency pot?

1. `frequency = 10 × 100^(pot ÷ 4095)`: 10 Hz at one end, 1000 Hz at the other (in the sketch: `MIN_FREQ_HZ * pow(MAX_FREQ_HZ / MIN_FREQ_HZ, reading / 4095.0)`)
2. Middle of the turn (2048): `10 × 100^(0.5) = 100 Hz`. A quarter turn: `10 × 100^(0.25) ≈ 31.6 Hz`
3. Each equal bit of turn multiplies the frequency by the same amount, so 10 to 100 Hz gets half the turn, like 100 to 1000 Hz
4. A straight `map()` would squeeze 10 to 100 Hz into the first 9 % of the turn

## Worked example: how exact is the frequency?

1. Step time = `1,000,000 ÷ (256 × f)`. At 100 Hz: `39.06 → 39 µs` → period `256 × 39 = 9,984 µs` → `100.2 Hz`
2. At 1000 Hz: `3.9 → 3 µs` → `768 µs` → 1302 Hz, if each `loop()` pass took no time
3. In reality each pass takes a few microseconds and late steps push the next ones later, which pulls the other way. So the real frequency is approximate and the error grows towards the top; measure it
4. For exact high frequencies, use DMA or the DAC's built-in cosine generator

## Three design details

- **Order.** The button cycles SINE → SQUARE → TRIANGLE → SINE, the order the task asks for.
- **Triangle frequency.** A triangle that counted up through all 256 samples and back down through 256 would have 512 steps per period and run at half the set frequency. This triangle climbs for 128 samples and falls for 128, so all three shapes have 256 steps per period.
- **Message.** The sketch calls `Serial.begin(115200)` in `setup()` and prints the name of each new waveform, for example `Waveform: SQUARE`. Printing without `Serial.begin()` would show nothing.
