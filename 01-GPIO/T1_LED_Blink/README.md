# T1: LED Blink

Blink one LED forever: 400 ms ON, 800 ms OFF, about 10 mA.

## The task

Connect one LED to a GPIO pin through a resistor. The LED turns ON for 400 ms, then OFF for 800 ms, and repeats forever.

Choose the resistor so that about 10 mA flows through the LED while it is ON.

## Rules

| Rule | Value |
|---|---|
| LED ON time | 400 ms |
| LED OFF time | 800 ms |
| LED current | about 10 mA |
| Repeat | forever |

## Parts

- ESP32 DevKit
- 1 LED
- 1 resistor, 150 Ω
- jumper wires

## Wiring

| ESP32 pin | Connection (from the pin outward) |
|---|---|
| GPIO4 | 150 Ω resistor → LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

Source wiring: when the pin is HIGH it pushes current out, through the resistor and LED, into GND. For the opposite (sink) wiring, see `T1_LED_Blink_Sink` below.

## Upload and run

This task has more than one version of the program. Each runs on its own on the same board and wiring; flash the one you want.

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`T1_LED_Blink`](T1_LED_Blink/T1_LED_Blink.ino) | Source wiring (HIGH = LED on) | not used |
| [`T1_LED_Blink_Sink`](T1_LED_Blink_Sink/T1_LED_Blink_Sink.ino) | Sink wiring (LOW = LED on) | not used |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### T1_LED_Blink_Sink: Sink wiring version

This version wires the LED the other way round: 3.3 V → 150 Ω → LED → GPIO4. Now the pin *sinks* the current, so **LOW turns the LED ON** and HIGH turns it OFF. Only the HIGH and LOW values swap; everything else stays the same.

| ESP32 pin | Connection (from the pin outward) |
|---|---|
| GPIO4 | LED → 150 Ω resistor → 3.3 V |

Here the LED is reversed: its short leg (cathode) goes to the pin and its long leg (anode) to the resistor and 3.3 V.

Sink: current flows from 3.3 V into the pin. LOW = ON (active-low).

## Timing

High = 3.3 V on the pin = LED ON. One full cycle is 1200 ms.

## Worked example: the resistor for 10 mA

1. Pin voltage when HIGH: `3.3 V`
2. LED forward voltage (VF, from the LED's datasheet): `1.8 V`
3. Voltage left for the resistor: `3.3 − 1.8 = 1.5 V`
4. Ohm's law, R = V ÷ I: `1.5 V ÷ 0.010 A = 150 Ω`
5. 150 Ω is a standard value. With a 2.0 V red LED it gives `1.3 ÷ 150 = 8.7 mA`, slightly under 10 mA, which is the safe side.

Real-world detail: a GPIO pin has a little internal resistance (roughly 20 to 40 Ω), so at 10 mA the HIGH level sags to about 3.0 V. The real current ends up a bit lower than the math. That is fine for an LED.
