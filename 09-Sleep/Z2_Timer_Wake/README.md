# Z2: Sleep Mode with Periodic Wake-Up

Wake once a minute, read and print the pot, and deep sleep again.

## The task

Keep the microcontroller in a power-saving mode and wake it once every minute to read a potentiometer and print the value. Pick the wake-up method that saves the most power while keeping good timing.

The sketch uses **deep sleep** with the **RTC timer** as the wake-up source, set to 60 s, and reads the pot on GPIO34 once per wake-up.

## Rules

| Rule | Value |
|---|---|
| Sleep | deep sleep |
| Wake-up | RTC timer, 60 s (60,000,000 µs) |
| Each wake-up | ADC value: N |
| Before sleeping | Entering deep sleep for 60 seconds... |

## Parts

- ESP32 DevKit
- 1 potentiometer
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

## Upload and run

Open `Z2_Timer_Wake/Z2_Timer_Wake.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## How exact is "every minute"?

1. The timer starts when the chip falls asleep, so one cycle = 60 s of sleep + the time awake (boot about 0.3 s + the 100 ms pause + printing)
2. The sleep timer runs from the internal RTC oscillator (about 150 kHz). It is calibrated at start-up, but its speed changes with temperature, so a long sleep can end early or late
3. For better timing: subtract the awake time from the next sleep (a variable marked `RTC_DATA_ATTR` keeps its value through deep sleep), or fit an external 32.768 kHz crystal

## Power, worked out

1. One measured DevKit drew about 50 mA awake and 10 mA asleep for the whole board (USB chip, regulator and power LED included); a USB power meter shows yours
2. Awake 0.4 s per 60 s at 50 mA, asleep 59.6 s at 10 mA → average `(50 × 0.4 + 10 × 59.6) ÷ 60 ≈ 10.3 mA`
3. The ESP32 chip alone sleeps at about 10 µA: `(50 × 0.4 + 0.01 × 59.6) ÷ 60 ≈ 0.34 mA`, 30 times less
4. So on a battery, the board around the chip decides the battery life

**Serial Monitor and deep sleep:** the USB-to-UART chip stays powered, so the monitor stays connected and shows each wake-up's lines. If the first line after a wake-up is cut off, the 100 ms pause after `Serial.begin()` is too short for your PC; make it longer.
