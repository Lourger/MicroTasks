# Z1: Sleep Mode with Button Press Wake-Up

Deep sleep until a button is pressed, then read and print the pot, and sleep again.

## The task

Keep the microcontroller in a power-saving mode until a button is pressed. On each press: wake up, read a potentiometer, print the ADC value, and go back to sleep.

The sketch uses **deep sleep**, woken by **ext0** (one RTC pin, here GPIO33, going LOW). After each button wake-up it reads the pot on GPIO34 and prints `ADC value: N`. Before sleeping again it waits for the button to be released (see *Why wait for the release?*).

## Rules

| Rule | Value |
|---|---|
| Sleep | deep sleep |
| Wake-up | ext0: GPIO33 LOW (button to GND, pull-up) |
| After a button wake-up | ADC value: N (pot on GPIO34) |
| Before sleeping | Entering deep sleep... |

## Parts

- ESP32 DevKit
- 1 push button
- 1 potentiometer
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO33 | INPUT_PULLUP | wake button → GND |
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

The wake-up button must be on an RTC GPIO (0, 2, 4, 12 to 15, 25 to 27, 32 to 39): in deep sleep only the RTC part of the chip is still watching pins.

## Upload and run

Open `Z1_Button_Wake/Z1_Button_Wake.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## What happens, start to finish

| Step | Chip state | What runs |
|---|---|---|
| 1. power on | awake | setup(): wake cause is "not a sleep wake-up", so no reading; sleep |
| 2. sleeping | deep sleep (CPUs and RAM off) | nothing; the RTC part watches GPIO33 |
| 3. button pressed | waking: a full restart | setup() from the top: wake cause EXT0 → read and print |
| 4. button released | awake | setup() continues: sleep again |

**Deep sleep restarts the program.** When the chip wakes, it boots like after a reset: `setup()` runs from the top, and normal variables start fresh. `loop()` is never reached. Only the RTC memory (variables marked `RTC_DATA_ATTR`) survives, for example `RTC_DATA_ATTR int bootCount = 0;`.

## Why wait for the release?

ext0 wakes on a LOW *level*, not on a press. If the sketch only waited a fixed 50 ms and the button were still held, the chip would wake again at once and print a second reading, and keep doing that until you let go. So the sketch waits until the pin is HIGH again, plus 50 ms for bounce.

**Serial Monitor and deep sleep:** the USB-to-UART chip stays powered, so the monitor stays connected and shows each wake-up's lines. If the first line after a wake-up is cut off, the 100 ms pause after `Serial.begin()` is too short for your PC; make it longer.
